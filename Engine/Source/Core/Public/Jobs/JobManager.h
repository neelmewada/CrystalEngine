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

			JobManager* owner = nullptr;

			String name = "";
			JobThreadTag tag = JOB_THREAD_WORKER;

			Mutex mutex{};
			bool isMainThread = false;

			int index = 0;
			ThreadId threadId = 0;

			Atomic<bool> isActive = true;
			Atomic<bool> isAvailable = true;

			Atomic<bool> deactivate = false;
			Atomic<bool> isSleeping = false;

			/// Variable storage that is created locally on the thread
			Atomic<WorkThreadLocal*> threadLocal = nullptr;

			Thread thread;
		};

		// - Public API -

		inline int GetNumThreads() const { return numThreads; }

		/// Deactivates all worker threads after they're done executing current job but does *NOT* wait on this thread for them to terminate.
		void DeactivateWorkers();

		/// Deactivates all worker threads after they're done executing current job and wait for them on this thread to terminate.
		void DeactivateWorkersAndWait();

		/// Waits for all worker threads to complete execution until all jobs are finished, and deactivates & terminates all worker threads.
		void Complete();

		int GetCurrentJobThreadIndex();

	private:

		void SpawnWorkThreads(const JobManagerDesc& desc);

		void ProcessJobsWorker(WorkThread* threadInfo);
		void ProcessJobsInternal(WorkThread* threadInfo);

		void Process(Job* job);

		void EnqueueJob(Job* job);

		int FixNumThreads(const JobManagerDesc& desc);

	private:
		// - Fields -

		String name{};

		Array<WorkThread*> workerThreads{};
		int numThreads = 0;

		Atomic<bool> complete = false;
		Atomic<bool> threadsCreated = false;

		Mutex mutex{};

		friend class Job;

	};

} // namespace CE
