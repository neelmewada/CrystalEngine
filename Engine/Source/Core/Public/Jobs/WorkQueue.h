#pragma once

#include <deque>
#include <shared_mutex>

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
			return queue.empty();
		}

	private:

		friend class JobManager;

		Job* TrySteal();

		Job* LocalPop();
		void LocalPush(Job* job);

		std::shared_mutex mutex{};

		std::deque<Job*> queue{};
	};
    
} // namespace CE
