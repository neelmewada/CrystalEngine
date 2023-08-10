#pragma once

namespace CE
{
	class JobManager;

	class CORE_API JobContext
	{
	public:

		JobContext(JobManager* manager)
			: manager(manager)
		{

		}

		static void SetGlobalContext(JobContext* context);
		static JobContext* GetGlobalContext();

		inline JobManager* GetJobManager() const
		{
			return manager;
		}

	private:

		JobManager* manager = nullptr;
	};
    
} // namespace CE
