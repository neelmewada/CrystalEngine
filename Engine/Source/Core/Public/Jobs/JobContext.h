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

		/// Ideally, you should only have one context pushed
		static void PushGlobalContext(JobContext* context);

		static void PopGlobalContext();

		static JobContext* GetGlobalContext();

		inline JobManager* GetJobManager() const
		{
			return manager;
		}

	private:

		JobManager* manager = nullptr;
	};
    
} // namespace CE
