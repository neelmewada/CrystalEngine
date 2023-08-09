#pragma once

namespace CE
{
	class TypeInfo;

	struct JobData
	{
		void* userData = 0;
		u64 userDataSize = 0;
		TypeInfo* userType = nullptr;
	};

	struct IJob
	{
		virtual void ExecuteJob(JobData& data) = 0;
		virtual void FinishJob(const JobData& data) {}
	};

	class CORE_API Job
	{
	public:

		Job(const JobData& data = {});

		void Execute();

	private:

		friend class JobQueue;
		friend class WorkerThread;

		IJob* jobImpl = nullptr;
		Job* parent = nullptr;
		std::atomic<int> unfinishedJobs = 0;

		JobData data{};
	};
}

