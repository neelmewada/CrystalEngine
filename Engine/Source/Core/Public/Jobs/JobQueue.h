#pragma once

namespace CE
{

	class CORE_API JobQueue
	{
	public:

		JobQueue();

		Job* Steal();

		inline bool IsEmpty() const
		{
			return jobs.IsEmpty();
		}

	private:

		friend class WorkerThread;

		Job* Pop();
		void Push(Job* job);

		Mutex mutex{};

		Array<Job*> jobs{};
	};
    
} // namespace CE
