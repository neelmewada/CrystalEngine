#include "CoreMinimal.h"

namespace CE
{
	JobManager::JobManager(const String& name, int numThreads)
		: name (name)
		, numThreads(FixNumThreads(numThreads))
	{
		SpawnWorkThreads();
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

	int JobManager::FixNumThreads(int numThreads)
	{
		if (numThreads == 0)
			numThreads = Thread::GetHardwareConcurrency();
		if (numThreads < 2)
			numThreads = 2;
		if (numThreads > 64)
			numThreads = 64;

		return numThreads;
	}

	void JobManager::SpawnWorkThreads()
	{
		if (!workerThreads.IsEmpty())
			return;

		numThreads = FixNumThreads(numThreads);

		for (int i = 0; i < numThreads; i++)
		{
			WorkThread* worker = new WorkThread();
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
		
		while (threadInfo->isActive)
		{
			// Deactivate after current job if required
			if (threadInfo->deactivate.load(std::memory_order_acquire))
				threadInfo->isActive = false;

			Job* job = threadInfo->threadLocal.load()->queue.LocalPop();

			if (job == nullptr && numWorkerThreads > 1)
			{
				// TODO: Try stealing from other queue
				int random = Random::Range(0, numWorkerThreads - 1);
				WorkThread* worker = workerThreads[random];

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

				job = worker->threadLocal.load()->queue.TrySteal();

				if (job == nullptr) // Could not steal job
				{
					// Yield
					continue;
				}

				if (job != nullptr)
				{
					if (threadInfo->isAvailable)
						threadInfo->isAvailable = false;

					Process(job);

					if (!threadInfo->isAvailable)
						threadInfo->isAvailable = true;

					continue;
				}
			}

			if (job != nullptr)
			{
				if (threadInfo->isAvailable)
					threadInfo->isAvailable = false;

				Process(job);
			}

			if (!threadInfo->isAvailable)
				threadInfo->isAvailable = true;
		}
	}

	void JobManager::Process(Job* job)
	{
		Job* dependent = job->GetDependent();
		bool isAutoDelete = job->IsAutoDelete();

		job->Process();

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

	void JobManager::QueueJob(Job* job)
	{
		if (workerThreads.IsEmpty() || workerThreads[0]->threadLocal == nullptr)
			return;

		workerThreads[0]->threadLocal.load()->queue.GlobalInsert(job);
	}

	JobManager::WorkThread::~WorkThread()
	{
		delete threadLocal;
		threadLocal = nullptr;
	}

} // namespace CE
