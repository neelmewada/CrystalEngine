#pragma once

/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */


#include <queue>

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

			/// Deactivates the worker thread, but doesn't wait for it to finish
			void Deactivate();

			/// Deactivates the worker thread and waits for it to finish
			void DeactivateAndWait();

			void Complete();

			void Awake();
			void Sleep();

			bool IsLocalQueueEmpty();

			JobManager* owner = nullptr;
			bool isWorker = false;

			Job* currentJob = nullptr;

			String name = "";
			JobThreadTag tag = JOB_THREAD_WORKER;

			Mutex mutex{};
			bool isMainThread = false;

			int index = 0;
			ThreadId threadId = 0;

			Atomic<bool> isAvailable = false;

			Atomic<bool> deactivate = false;
			Atomic<bool> complete = false;

			std::binary_semaphore sleepEvent{ 0 };

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

		int FixNumThreads(const JobManagerDesc& desc);

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

		void AwakeOrSleepWorkers();

		bool AwakeWorker(WorkThread* specificWorker = nullptr);

	private:
		// - Fields -

		String name{};
		JobThreadTag defaultTag = JOB_THREAD_WORKER;

		Array<WorkThread*> workerThreads{};
		Atomic<int> numThreads = 0;

		Atomic<bool> threadsCreated = false;
		Atomic<int> totalJobsInQueue = 0;
		Atomic<int> numAvailableWorkers = 0;

		std::deque<Job*> globalQueue{};
		Mutex jobManagerMutex{};

		friend class Job;

	};

} // namespace CE
