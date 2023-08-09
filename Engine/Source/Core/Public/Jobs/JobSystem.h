#pragma once

namespace CE
{
	class WorkerThread;

	class CORE_API JobSystem
	{
	public:

		JobSystem(int numThreads = 0);
		~JobSystem();

		void Dispatch(IJob* job, const JobData& jobData);

	private:

		Mutex mutex{};

		Array<WorkerThread*> workers{};
	};

} // namespace CE
