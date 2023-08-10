#include "CoreMinimal.h"

namespace CE
{
	WorkQueue::WorkQueue()
	{

	}

	Job* WorkQueue::TrySteal()
	{
		LockGuard<Mutex> guard{ mutex };

		Job* job = nullptr;

		if (queue.NonEmpty())
		{
			job = queue[0];
			queue.RemoveAt(0);
		}

		return job;
	}

	void WorkQueue::GlobalInsert(Job* job)
	{
		LockGuard<Mutex> guard{ mutex };

		if (job != nullptr)
		{
			//queue.InsertAt(0, job);
			queue.Add(job);
		}
	}

	Job* WorkQueue::LocalPop()
	{
		if (queue.IsEmpty())
			return nullptr;
		auto result = queue.Top();
		queue.Pop();
		return result;
	}

	void WorkQueue::LocalPush(Job* job)
	{
		queue.Push(job);
	}

} // namespace CE
