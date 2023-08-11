#include "CoreMinimal.h"

namespace CE
{
	static Array<JobContext*> gJobContext = {};

	void JobContext::PushGlobalContext(JobContext* context)
	{
		gJobContext.Push(context);
	}

	void JobContext::PopGlobalContext()
	{
		gJobContext.Pop();
	}

	JobContext* JobContext::GetGlobalContext()
    {
		if (gJobContext.IsEmpty())
			return nullptr;
        return gJobContext.Top();
    }

} // namespace CE
