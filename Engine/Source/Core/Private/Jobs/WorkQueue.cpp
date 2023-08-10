#include "CoreMinimal.h"

namespace CE
{
	WorkQueue::WorkQueue()
	{

	}

	Job* WorkQueue::TrySteal()
	{

		if (mutex.try_lock())
		{
			Job* job = nullptr;
			if (!queue.empty())
			{
				job = queue.front();
				queue.pop_front();
			}
			mutex.unlock();
			return job;
		}

		return nullptr;
	}

	void WorkQueue::GlobalInsert(Job* job)
	{
		std::lock_guard<std::shared_mutex> guard{ mutex };

		if (job != nullptr)
		{
			queue.push_back(job);
		}
	}

	Job* WorkQueue::LocalPop()
	{
		std::lock_guard<std::shared_mutex> guard{ mutex };

		if (queue.empty())
			return nullptr;
		auto result = queue.front();
		queue.pop_front();
		return result;
	}

	void WorkQueue::LocalPush(Job* job)
	{
		std::lock_guard<std::shared_mutex> guard{ mutex };

		queue.push_front(job);
	}

} // namespace CE
