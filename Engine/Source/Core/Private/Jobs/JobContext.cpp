#include "CoreMinimal.h"

namespace CE
{
	static JobContext* gJobContext = nullptr;

	void JobContext::SetGlobalContext(JobContext* context)
	{
		gJobContext = context;
	}

	JobContext* JobContext::GetGlobalContext()
    {
        return gJobContext;
    }

} // namespace CE
