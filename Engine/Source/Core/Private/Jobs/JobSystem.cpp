#include "CoreMinimal.h"

namespace CE
{
	JobSystem::JobSystem(int numThreads)
	{
		if (numThreads <= 0)
			numThreads = std::thread::hardware_concurrency();
		if (numThreads < 2)
			numThreads = 2;
		if (numThreads > 128)
			numThreads = 128; // Hard limit of 128 threads

		for (int i = 0; i < numThreads; i++)
		{
			WorkerThread* thread = new WorkerThread(this);
			if (i > 0)
				thread->InitNewThread();
			else
				thread->InitMainThread();

			workers.Add(thread);
		}
	}

	JobSystem::~JobSystem()
	{
		for (int i = 0; i < workers.GetSize(); i++)
		{
			workers[i]->DeactivateAndWait();
			delete workers[i];
		}

		workers.Clear();
	}

	void JobSystem::Dispatch(IJob* job, const JobData& jobData)
	{
		if (job == nullptr)
			return;

		workers[0]->Dispatch(job, jobData);
	}

} // namespace CE
