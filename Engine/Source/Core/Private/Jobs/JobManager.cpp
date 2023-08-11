#include "CoreMinimal.h"

namespace CE
{
	JobManager::JobManager(const String& name, const JobManagerDesc& desc)
		: name (name)
		, numThreads(FixNumThreads(desc))
	{
		SpawnWorkThreads(desc);
	}

	JobManager::~JobManager()
	{
		for (int i = 0; i < workerThreads.GetSize(); i++)
		{
			workerThreads[i]->Deactivate();
			delete workerThreads[i]; // deleting will terminate the worker thread
		}

		workerThreads.Clear();
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

		ProcessJobsInternal(threadInfo);

		currentThreadInfo = nullptr;
	}

	void JobManager::ProcessJobsInternal(WorkThread* threadInfo)
	{
		int numWorkerThreads = workerThreads.GetSize();

reset_thread:
		
		while (threadInfo->isActive)
		{
			// Deactivate only after current job is finished
			if (threadInfo->deactivate.load(std::memory_order_acquire))
				threadInfo->isActive.store(false, std::memory_order_release);
			
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
					continue;
				}

				if (worker->threadLocal.load() == nullptr) // Thread's local storage not initialized yet
				{
					// Yield
					continue;
				}
				
				// If thread is available & not executing anything (idle), then don't steal from it
				if (worker->isAvailable.load(std::memory_order_acquire))
				{
					// Yield
					continue;
				}

				// Try stealing from another thread's queue if the filter tag is satisfied
				job = worker->threadLocal.load()->queue.TrySteal(threadInfo->tag);

				if (job == nullptr) // Could not steal a job
				{
					// Yield
					continue;
				}
			}

			if (job != nullptr)
			{
				if (threadInfo->isAvailable)
					threadInfo->isAvailable.store(false, std::memory_order_release);

				Process(job);
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

		job->SetDependentCountAndFlags(Job::FLAGS_FINISHED);

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

			for (int i = 0; i < workerThreads.GetSize(); i++)
			{
				bool threadComplete = workerThreads[i]->isAvailable.load(std::memory_order_acquire) && workerThreads[i]->threadLocal.load()->queue.IsEmpty();

				if (!threadComplete)
					allThreadsCompleted = false;
			}

			if (allThreadsCompleted)
				break;
		}

		// Deactivate all worker threads
		DeactivateWorkersAndWait();
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
