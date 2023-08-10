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
			inline void DeactivateAndWait()
			{
				Deactivate();
				if (thread.IsJoinable())
					thread.Join();
			}

			JobManager* owner = nullptr;

			Mutex mutex{};
			bool isMainThread = false;

			int index = 0;
			ThreadId threadId = 0;

			Atomic<bool> isActive = true;
			Atomic<bool> isAvailable = true;

			/// Variable storage that is created locally on the thread
			Atomic<WorkThreadLocal*> threadLocal = nullptr;

			Thread thread;
		};

		// - Public API -

		inline int GetNumThreads() const { return numThreads; }

		/// Deactivates all worker threads but does NOT wait for them to finish
		void DeactivateWorkers();

		/// Deactivates all worker threads and wait for them to finish
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
