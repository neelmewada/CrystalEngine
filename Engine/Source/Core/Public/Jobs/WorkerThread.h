#pragma once

namespace CE
{
	class JobSystem;
	class JobQueue;

	class CORE_API WorkerThread
	{
	public:

		WorkerThread(JobSystem* owner);
		~WorkerThread();

		inline JobQueue& GetQueue() { return queue; }

		void Deactivate();
		void DeactivateAndWait();

	private:

		friend class JobSystem;

		void Dispatch(IJob* job, const JobData& jobData);

		void Run();

		void InitNewThread();
		void InitMainThread();

		Job* GetJob();

		JobSystem* owner = nullptr;

		JobQueue queue{};

		Mutex mutex{};
		bool isMainThread = false;
		bool isActive = true;

		Thread thread;
	};
    
} // namespace CE
