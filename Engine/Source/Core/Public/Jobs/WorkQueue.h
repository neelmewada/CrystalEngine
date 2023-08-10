#pragma once

namespace CE
{
	class Job;

	class CORE_API WorkQueue final
	{
	public:

		WorkQueue();

		// Insert globally using FIFO order
		void GlobalInsert(Job* job);

		inline bool IsEmpty() const
		{
			return queue.IsEmpty();
		}

	private:

		friend class JobManager;

		Job* TrySteal();

		Job* LocalPop();
		void LocalPush(Job* job);

		Mutex mutex{};

		Array<Job*> queue{};
	};
    
} // namespace CE
