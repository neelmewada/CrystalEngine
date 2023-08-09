#include "CoreMinimal.h"

namespace CE
{
	Job::Job(const JobData& data)
		: data(data)
	{

	}

	void Job::Execute()
	{
		if (jobImpl)
		{
			jobImpl->ExecuteJob(data);
			jobImpl->FinishJob(data);
		}
	}
}

