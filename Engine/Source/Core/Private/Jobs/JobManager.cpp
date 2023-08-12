#include "CoreMinimal.h"

namespace CE
{
	JobManager::JobManager(const String& name, const JobManagerDesc& desc)
		: name (name)
		, defaultTag(desc.defaultTag)
		, numThreads(FixNumThreads(desc))
	{
		SpawnWorkThreads(desc);
	}

	JobManager::~JobManager()
	{
		mutex.Lock();

		for (int i = 0; i < workerThreads.GetSize(); i++)
		{
			workerThreads[i]->Deactivate();
			delete workerThreads[i]; // deleting will terminate the worker thread
		}

		workerThreads.Clear();

		mutex.Unlock();
	}

	JobManager::WorkThread::~WorkThread()
	{
		delete threadLocal;
		threadLocal = nullptr;
	}

	void JobManager::WorkThread::Suspend()
	{
		isActive.store(false, std::memory_order_release);
	}

	void JobManager::WorkThread::Deactivate()
	{
		deactivate.store(true, std::memory_order_release);
	}

	void JobManager::WorkThread::DeactivateAndWait()
	{
		Deactivate();
		if (thread.IsJoinable())
			thread.Join();
	}

	void JobManager::WorkThread::Complete()
	{
		complete.store(true, std::memory_order_release);
		if (thread.IsJoinable())
			thread.Join();
	}

	int JobManager::FixNumThreads(const JobManagerDesc& desc)
	{
		int totalThreads = desc.totalThreads;

		if (totalThreads == 0)
			totalThreads = Thread::GetHardwareConcurrency() - 1;
		if (totalThreads < 2)
			totalThreads = 2;
		if (totalThreads > 64)
			totalThreads = 64;

		return totalThreads;
	}

	void JobManager::SpawnWorkThreads(const JobManagerDesc& desc)
	{
		if (!workerThreads.IsEmpty())
			return;

		for (int i = 0; i < numThreads; i++)
		{
			JobThreadDesc threadDesc = {};
			if (i < desc.threads.GetSize())
				threadDesc = desc.threads[i];

			WorkThread* worker = new WorkThread();
			worker->name = threadDesc.threadName;
			worker->tag = threadDesc.tag;
			worker->owner = this;
			worker->isMainThread = false;
			worker->index = i;

			worker->thread = Thread([this, worker]
				{
					this->ProcessJobsWorker(worker);
				});

			while (worker->threadLocal == nullptr)
			{
				// Wait till thread is initialized
			}
			
			workerThreads.Add(worker);
		}

		threadsCreated.store(true, std::memory_order_release);
	}

	CE_THREAD_LOCAL JobManager::WorkThread* currentThreadInfo = nullptr;

	int JobManager::GetCurrentJobThreadIndex()
	{
		if (currentThreadInfo == nullptr)
			return -1;
		return currentThreadInfo->index;
	}

	void JobManager::SuspendJobUntilReady(Job* job)
	{
		auto currentWorkThread = GetCurrentOrCreateThread();
		CE_ASSERT(currentWorkThread != nullptr, "Could not find current work thread"); // Should never happen

		ProcessJobsInternal(currentThreadInfo, job);
	}

	JobManager::WorkThread* JobManager::GetCurrentOrCreateThread()
	{
		auto threadInfo = currentThreadInfo;

		if (!threadInfo)
		{
#ifndef PAL_TRAIT_BUILD_MONOLITHIC
			threadInfo = FindAndSetCurrentWorkThread();
#endif
			if (!threadInfo)
			{
				threadInfo = new WorkThread();
				threadInfo->name = "Dynamic";
				threadInfo->tag = defaultTag;
				threadInfo->owner = this;
				threadInfo->isMainThread = false;
				threadInfo->index = numThreads.load(std::memory_order_acquire);
				threadInfo->threadId = Thread::GetCurrentThreadId();

				mutex.Lock();
				workerThreads.Add(threadInfo);
				mutex.Unlock();

				numThreads.fetch_add(1, std::memory_order_acq_rel);
			}
		}

		return threadInfo;
	}

#ifndef PAL_TRAIT_BUILD_MONOLITHIC
	JobManager::WorkThread* JobManager::FindAndSetCurrentWorkThread()
	{
		auto thisThreadId = Thread::GetCurrentThreadId();

		int numThreads = this->numThreads.load(std::memory_order_acquire);

		for (int i = 0; i < numThreads; i++)
		{
			if (workerThreads[i]->threadId == thisThreadId)
			{
				auto info = workerThreads[i];
				currentThreadInfo = info;
				return info;
			}
		}

		return nullptr;
	}
#endif

	void JobManager::ProcessJobsWorker(WorkThread* threadInfo)
	{
		threadInfo->threadId = Thread::GetCurrentThreadId();
		threadInfo->threadLocal = new WorkThreadLocal();

		// Wait for all threads to initialize. Otherwise, we will be stealing jobs from uninitialized threads
		while (!threadsCreated.load(std::memory_order_acquire))
		{
			// Wait till all threads are initialized
		}

		currentThreadInfo = threadInfo;

		ProcessJobsInternal(threadInfo, nullptr);

		currentThreadInfo = nullptr;
	}

	void JobManager::ProcessJobsInternal(WorkThread* threadInfo, Job* suspendedJob)
	{
		
		while (threadInfo->isActive)
		{
			// Check if suspended job is ready to be executed
			if (suspendedJob != nullptr && suspendedJob->GetDependentCount() == 0)
			{
				return;
			}

			int numWorkerThreads = numThreads.load(std::memory_order_acquire);

			// Deactivate only after current job is finished
			if (threadInfo->deactivate.load(std::memory_order_acquire))
				threadInfo->isActive.store(false, std::memory_order_release);

			auto yield = [=]
				{
					if (suspendedJob == nullptr && threadInfo->complete.load(std::memory_order_acquire))
						threadInfo->isActive.store(false, std::memory_order_release);
				};
			
			if (threadInfo->threadLocal == nullptr) 
			{
				break;
			}

			Job* job = threadInfo->threadLocal.load()->queue.LocalPop();

			if (job == nullptr && numWorkerThreads > 1)
			{
				// TODO: Try stealing from other queue
				int random = Random::Range(0, numWorkerThreads - 1);
				WorkThread* worker = workerThreads[random];

				// If we arrive to this same thread, move backward or forward by 1 index
				if (worker == threadInfo || worker == nullptr)
				{
					if (random > 0)
						random--;
					else if (random < numWorkerThreads - 1)
						random++;
					else
						continue; // Yield

					worker = workerThreads[random];
				}

				if (worker == threadInfo || worker == nullptr)
				{
					// Yield
					yield();
					continue;
				}

				if (worker->threadLocal.load() == nullptr) // Thread's local storage not initialized yet
				{
					// Yield
					yield();
					continue;
				}
				
				// If thread is available & not executing anything (idle), then don't steal from it
				if (worker->isAvailable.load(std::memory_order_acquire))
				{
					// Yield
					yield();
					continue;
				}

				// Try stealing from another thread's queue if the filter tag is satisfied
				job = worker->threadLocal.load()->queue.TrySteal(threadInfo->tag);

				if (job == nullptr) // Could not steal a job
				{
					// Yield
					yield();
					continue;
				}
			}

			if (job != nullptr)
			{
				if (threadInfo->isAvailable)
					threadInfo->isAvailable.store(false, std::memory_order_release);
				threadInfo->currentJob = job;

				Process(job);

				threadInfo->currentJob = nullptr;
			}

			if (!threadInfo->isAvailable)
				threadInfo->isAvailable.store(true, std::memory_order_release);
		}

	}

	void JobManager::Process(Job* job)
	{
		Job* dependent = job->GetDependent();
		bool isAutoDelete = job->IsAutoDelete();

		job->Process();

		auto oldFlags = job->GetDependentCountAndFlags();
		job->SetDependentCountAndFlags(oldFlags | Job::FLAGS_FINISHED);

		job->Finish();

		if (isAutoDelete)
		{
			delete job;
		}

		if (dependent)
		{
			dependent->DecrementDependentCount();
		}
	}

	int JobManager::GetNumThreads()
	{
		auto size = workerThreads.GetSize();
		workerThreads;
		return size;
	}

	void JobManager::DeactivateWorkers()
	{
		if (workerThreads.IsEmpty())
			return;

		for (int i = 0; i < workerThreads.GetSize(); i++)
		{
			workerThreads[i]->Deactivate();
		}
	}

	void JobManager::DeactivateWorkersAndWait()
	{
		if (workerThreads.IsEmpty())
			return;

		for (int i = 0; i < workerThreads.GetSize(); i++)
		{
			workerThreads[i]->DeactivateAndWait();
		}
	}

	void JobManager::Complete()
	{
		if (workerThreads.IsEmpty())
			return;

		complete.store(true, std::memory_order_release);

		// Wait for all workers to finish jobs
		while (true)
		{
			bool allThreadsCompleted = true;

			for (int i = 0; i < numThreads; i++)
			{
				bool threadComplete = workerThreads[i]->isAvailable.load(std::memory_order_acquire) && workerThreads[i]->threadLocal.load()->queue.IsEmpty();

				if (!threadComplete)
					allThreadsCompleted = false;
			}

			if (allThreadsCompleted)
				break;
		}

		// Complete() all worker threads
		for (int i = 0; i < workerThreads.GetSize(); i++)
		{
			workerThreads[i]->Complete();
		}
	}

	void JobManager::EnqueueJob(Job* job)
	{
		if (job == nullptr || workerThreads.IsEmpty())
			return;

		ThreadId callingThreadId = Thread::GetCurrentThreadId();
		bool workerFound = false;

		auto threadFilterTag = job->GetThreadFilter();

		// 1st attempt: Try to enqueue in an active worker of same threadId
		for (int i = 0; i < workerThreads.GetSize(); i++)
		{
			WorkThread* worker = workerThreads[i];
			if (worker->threadLocal == nullptr)
				continue;

			if (worker->isActive.load(std::memory_order_acquire) && worker->threadId == callingThreadId &&
				(threadFilterTag == JOB_THREAD_UNDEFINED || worker->tag == threadFilterTag))
			{
				workerFound = true;
				worker->threadLocal.load()->queue.GlobalInsert(job);
				break;
			}
		}

		if (workerFound)
			return;
		
		// 2nd attempt
		for (int i = 0; i < workerThreads.GetSize(); i++)
		{
			WorkThread* worker = workerThreads[i];
			if (worker->threadLocal == nullptr)
				continue;
			
			if (worker->isActive.load(std::memory_order_acquire) && (threadFilterTag == JOB_THREAD_UNDEFINED || worker->tag == threadFilterTag))
			{
				worker->threadLocal.load()->queue.GlobalInsert(job);
				break;
			}
		}
	}

} // namespace CE
