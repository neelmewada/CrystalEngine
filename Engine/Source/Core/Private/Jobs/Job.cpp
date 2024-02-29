#include "CoreMinimal.h"

namespace CE
{
	Job::Job(bool isAutoDelete, JobContext* context)
	{
		if (context)
		{
			this->context = context;
		}
		else
		{
			this->context = JobContext::GetGlobalContext();
		}

		CE_ASSERT(this->context != nullptr, "Job created without context");
		
		// initialize dependent count to 1 so the job doesn't start
		u32 countAndFlags = 1;
		if (isAutoDelete)
		{
			countAndFlags |= FLAGS_AUTO_DELETE;
		}

		SetDependentCountAndFlags(countAndFlags);
		ClearDependents();
	}

	Job::~Job()
	{

	}

	bool Job::IsInitialized()
	{
		return context != nullptr;
	}

	void Job::Start()
	{
		DecrementDependentCount();
	}

    void Job::Reset(bool clearDependent)
    {
		unsigned int countAndFlags = GetDependentCountAndFlags();
		// Clear flags & set dependent count to 1
		countAndFlags = (countAndFlags & (~(FLAGS_DEPENDENTCOUNT_MASK) & ~(FLAGS_CHILD_JOBS) & ~(FLAGS_FINISHED))) | 1;
		SetDependentCountAndFlags(countAndFlags);

		if (clearDependent)
		{
			ClearDependents();
		}
		else
		{
			LockGuard<SharedMutex> lock{ dependentJobsMutex };
			for (auto dependent : dependentJobs)
			{
				dependent->IncrementDependentCount();
			}
		}

		OnReset();
    }

	void Job::StartAsChild(Job* childJob)
	{
		childJob->SetDependentChild(this);
		childJob->Start();
	}

	void Job::WaitForChildren()
	{
		if (GetDependentCount() != 0)
		{
			// Suspend this job until all children are executed
			context->GetJobManager()->SuspendJobUntilReady(this);
		}
	}

    bool Job::IsAutoDelete()
	{
		return (GetDependentCountAndFlags() & (unsigned int)FLAGS_AUTO_DELETE) ? true : false;
	}

	bool Job::IsFinished()
	{
		return (GetDependentCountAndFlags() & (unsigned int)FLAGS_FINISHED) ? true : false;
	}

	bool Job::IsCancelled()
	{
		return (GetDependentCountAndFlags() & (unsigned int)FLAGS_CANCELLED) ? true : false;
	}

	void Job::SetThreadFilter(JobThreadTag threadTag)
	{
		this->threadFilter = threadTag;
	}

	void Job::SetDependent(Job* dependent)
	{
		if (dependent)
			dependent->IncrementDependentCount();
		StoreDependent(dependent);
	}

	void Job::SetDependentChild(Job* dependent)
	{
		if (dependent)
			dependent->IncrementDependentCountAndSetChildFlag();
		StoreDependent(dependent);
	}

	void Job::SetAutoDelete(bool set)
	{
		u32 oldCountAndFlags, newCountAndFlags;
		do
		{
			oldCountAndFlags = dependentCountAndFlags.load(std::memory_order_acquire);
			newCountAndFlags = (oldCountAndFlags & ~FLAGS_AUTO_DELETE) | (set ? FLAGS_AUTO_DELETE : FLAGS_NONE);
		} while (!dependentCountAndFlags.compare_exchange_weak(oldCountAndFlags, newCountAndFlags, std::memory_order_acq_rel, std::memory_order_acquire));
	}

	void Job::Complete()
	{
		while (!IsFinished())
		{
			// Wait until finished
		}
	}

	u32 Job::GetDependentCount()
	{
		return (GetDependentCountAndFlags() & (unsigned int)FLAGS_DEPENDENTCOUNT_MASK);
	}

	void Job::IncrementDependentCount()
	{
		dependentCountAndFlags.fetch_add(1, std::memory_order_acq_rel);
	}

	void Job::DecrementDependentCount()
	{
		u32 countAndFlags = dependentCountAndFlags.fetch_sub(1, std::memory_order_acq_rel);

		u16 count = (u16)(countAndFlags & FLAGS_DEPENDENTCOUNT_MASK);
		if (count == 1)
		{
			if (!IsFinished() && !(countAndFlags & FLAGS_CHILD_JOBS)) // NOT a suspended/parent job
			{
				// Enqueue the job for execution
				this->context->GetJobManager()->EnqueueJob(this);
			}
		}
	}

	void Job::IncrementDependentCountAndSetChildFlag()
	{
		u32 oldCountAndFlags, newCountAndFlags;
		do
		{
			oldCountAndFlags = dependentCountAndFlags.load(std::memory_order_acquire);
			int oldCount = oldCountAndFlags & FLAGS_DEPENDENTCOUNT_MASK;
			newCountAndFlags = (oldCountAndFlags & ~FLAGS_DEPENDENTCOUNT_MASK) | (oldCount + 1) | FLAGS_CHILD_JOBS;
		} while (!dependentCountAndFlags.compare_exchange_weak(oldCountAndFlags, newCountAndFlags, std::memory_order_acq_rel, std::memory_order_acquire));
	}

	//Job* Job::GetDependent()
	//{
	//	return this->dependent.load(std::memory_order_acquire);
	//}

	void Job::ClearDependents()
	{
		LockGuard<SharedMutex> lock{ dependentJobsMutex };
		dependentJobs.Clear();
	}

	void Job::StoreDependent(Job* dependent)
	{
		if (dependent == nullptr)
			return;

		LockGuard<SharedMutex> lock{ dependentJobsMutex };
		dependentJobs.Add(dependent);
		//this->dependent.store(dependent, std::memory_order_release);
	}

	void Job::SetDependentCountAndFlags(u32 countAndFlags)
	{
		dependentCountAndFlags.store(countAndFlags, std::memory_order_release);
	}

	u32 Job::GetDependentCountAndFlags()
	{
		return dependentCountAndFlags.load(std::memory_order_acquire);
	}
}

