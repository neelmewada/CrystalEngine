#include "Core.h"

namespace CE
{
	static std::atomic<u32> counter = 1;

    CORE_API u32 GetTheadLocalContextCounter()
    {
		return counter.fetch_add(1);
    }

} // namespace CE
