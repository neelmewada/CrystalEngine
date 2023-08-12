#pragma once

namespace CE
{
	class WorkThread;

	struct JobThreadDesc
	{
		String threadName = "";
		JobThreadTag tag = JOB_THREAD_WORKER;
	};

	struct JobManagerDesc
	{
		/// Default tag used to create new threads while running jobs
		JobThreadTag defaultTag = JOB_THREAD_WORKER;
		/// Set it to 0 to automatically select the most optimal number of threads
		int totalThreads = 0;
		/// Thread descriptions. Requirement: Size <= totalThreads.
		Array<JobThreadDesc> threads{};
	};

	class CORE_API JobManager final
	{
	public:

		JobManager(const String& name, const JobManagerDesc& desc);
		~JobManager();

		struct WorkThreadLocal
		{
			Mutex mutex{};
			WorkQueue queue{};
		};

		struct CORE_API WorkThread
		{
			~WorkThread();

			/// Suspend the worker thread, i.e. put to sleep
			void Suspend();

			/// Deactivates the worker thread, but doesn't wait for it to finish
			void Deactivate();

			/// Deactivates the worker thread and waits for it to finish
			void DeactivateAndWait();

			void Complete();

			JobManager* owner = nullptr;

			Job* currentJob = nullptr;

			String name = "";
			JobThreadTag tag = JOB_THREAD_WORKER;

			Mutex mutex{};
			bool isMainThread = false;

			int index = 0;
			ThreadId threadId = 0;

			Atomic<bool> isActive = true;
			Atomic<bool> isIdle = true;

			Atomic<bool> deactivate = false;
			Atomic<bool> complete = false;

			Atomic<bool> sleep = false;

			/// Variable storage that is created locally on the thread
			Atomic<WorkThreadLocal*> threadLocal = nullptr;

			Thread thread;
		};

		// - Public API -

		int GetNumThreads();

		/// Deactivates all worker threads after they're done executing current job but does *NOT* wait on this thread for them to terminate.
		void DeactivateWorkers();

		/// Deactivates all worker threads after they're done executing current job and wait for them on this thread to terminate.
		void DeactivateWorkersAndWait();

		/// Waits for all worker threads to complete execution until all jobs are finished, and deactivates & terminates all worker threads.
		void Complete();

		int GetCurrentJobThreadIndex();

	private:

		void SpawnWorkThreads(const JobManagerDesc& desc);

		/// Only meant to be called by a job class inside it's Process() function
		void SuspendJobUntilReady(Job* job);

		WorkThread* GetCurrentOrCreateThread();

#ifndef PAL_TRAIT_BUILD_MONOLITHIC
		WorkThread* FindAndSetCurrentWorkThread();
#endif

		void ProcessJobsWorker(WorkThread* threadInfo);
		void ProcessJobsInternal(WorkThread* threadInfo, Job* suspendedJob);

		void Process(Job* job);

		void EnqueueJob(Job* job);

		int FixNumThreads(const JobManagerDesc& desc);

	private:
		// - Fields -

		String name{};
		JobThreadTag defaultTag = JOB_THREAD_WORKER;

		Array<WorkThread*> workerThreads{};
		Atomic<int> numThreads = 0;

		Atomic<bool> complete = false;
		Atomic<bool> threadsCreated = false;

		Mutex mutex{};

		friend class Job;

	};

} // namespace CE
