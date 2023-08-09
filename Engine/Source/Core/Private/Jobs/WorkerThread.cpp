#include "CoreMinimal.h"

namespace CE
{
	WorkerThread::WorkerThread(JobSystem* owner)
		: owner(owner)
		, isActive(true)
	{

	}

	WorkerThread::~WorkerThread()
	{
		
	}

	void WorkerThread::InitNewThread()
	{
		isMainThread = false;
		thread = Thread([this] { this->Run(); });
	}

	void WorkerThread::InitMainThread()
	{
		isMainThread = true;
		Run();
	}

	void WorkerThread::Dispatch(IJob* job, const JobData& jobData)
	{
		if (job != nullptr)
		{
			Job* jobObject = new Job();
			jobObject->jobImpl = job;
			jobObject->parent = nullptr;
			jobObject->data = jobData;
			jobObject->unfinishedJobs = 1;
			queue.Push(jobObject);
		}
	}

	void WorkerThread::Run()
	{
		while (isActive)
		{
			Job* job = GetJob();
			if (job)
			{
				// Execute & finish
				job->Execute();
				delete job;
			}
			else
			{

			}
		}
	}

	void WorkerThread::Deactivate()
	{
		isActive = false;
	}

	void WorkerThread::DeactivateAndWait()
	{
		Deactivate();

		if (thread.IsJoinable())
			thread.Join();
	}

	Job* WorkerThread::GetJob()
	{
		Job* job = queue.Pop();

		if (job == nullptr)
		{
			// TODO: Implement job stealing
		}

		return job;
	}

} // namespace CE

