#pragma once

namespace CE
{
	class WorkThread;

	class CORE_API JobManager final
	{
	public:

		JobManager(const String& name, int numThreads = 0);
		~JobManager();

		struct WorkThreadLocal
		{
			Mutex mutex{};
			WorkQueue queue{};
		};

		struct CORE_API WorkThread
		{
			~WorkThread();

			/// Deactivates the worker thread, but doesn't wait for it to finish
			void Deactivate();

			/// Deactivates the worker thread and waits for it to finish
			void DeactivateAndWait();

			JobManager* owner = nullptr;

			Mutex mutex{};
			bool isMainThread = false;

			int index = 0;
			ThreadId threadId = 0;

			Atomic<bool> isActive = true;
			Atomic<bool> isAvailable = true;

			Atomic<bool> deactivate = false;

			/// Variable storage that is created locally on the thread
			Atomic<WorkThreadLocal*> threadLocal = nullptr;

			Thread thread;
		};

		// - Public API -

		inline int GetNumThreads() const { return numThreads; }

		/// Deactivates all worker threads after they're done executing current job but does *NOT* wait on this thread for them to finish
		void DeactivateWorkers();

		/// Deactivates all worker threads after they're done executing current job and wait for them on this thread to finish
		void DeactivateWorkersAndWait();

	private:

		void SpawnWorkThreads();

		void ProcessJobsWorker(WorkThread* threadInfo);
		void ProcessJobsInternal(WorkThread* threadInfo);

		void Process(Job* job);

		void QueueJob(Job* job);

		int FixNumThreads(int numThreads);

	private:
		// - Fields -

		String name{};

		Array<WorkThread*> workerThreads{};
		int numThreads = 0;

		Atomic<bool> threadsCreated = false;

		Mutex mutex{};

		friend class Job;

	};

} // namespace CE
