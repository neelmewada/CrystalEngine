#include "CoreMinimal.h"

namespace CE
{
	JobQueue::JobQueue()
	{

	}

	Job* JobQueue::Steal()
	{
		Job* job = nullptr;

		mutex.Lock();
		if (jobs.NonEmpty())
		{
			job = jobs[0];
			jobs.RemoveAt(0);
		}
		mutex.Unlock();

		return job;
	}

	Job* JobQueue::Pop()
	{
		if (jobs.IsEmpty())
			return nullptr;
		auto result = jobs.Top();
		jobs.Pop();
		return result;
	}

	void JobQueue::Push(Job* job)
	{
		jobs.Push(job);
	}

} // namespace CE
