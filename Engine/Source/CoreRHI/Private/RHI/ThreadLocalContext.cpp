#include "CoreRHI.h"

namespace CE::RHI
{
	static std::atomic<u32> counter = 1;

    CORERHI_API u32 GetTheadLocalContextCounter()
    {
		return counter.fetch_add(1);
    }

} // namespace CE::RHI
