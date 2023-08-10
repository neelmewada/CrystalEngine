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

		// initialize dependent count to 1 so the job doesn't start
		u32 countAndFlags = 1;
		if (isAutoDelete)
		{
			countAndFlags |= FLAGS_AUTO_DELETE;
		}

		SetDependentCountAndFlags(countAndFlags);
		SetDependent(nullptr);
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

	bool Job::IsAutoDelete()
	{
		return (GetDependentCountAndFlags() & (unsigned int)FLAGS_AUTO_DELETE) ? true : false;
	}

	void Job::SetDependent(Job* dependent)
	{
		if (dependent)
			dependent->IncrementDependentCount();
		StoreDependent(dependent);
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
			if (!(countAndFlags & FLAGS_CHILD_JOBS)) // NOT a child job
			{
				// Enqueue the job for execution
				this->context->GetJobManager()->QueueJob(this);
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

	Job* Job::GetDependent()
	{
		return this->dependent.load(std::memory_order_acquire);
	}

	void Job::StoreDependent(Job* dependent)
	{
		this->dependent.store(dependent, std::memory_order_release);
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

