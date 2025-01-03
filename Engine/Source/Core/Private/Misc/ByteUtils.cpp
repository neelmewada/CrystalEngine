
#include "CoreMinimal.h"

namespace CE
{

	CORE_API void SwapByteOrder(void* value, u32 length)
	{
		u8* ptr = (u8*)value;
		s32 top = length - 1;
		s32 bottom = 0;
		while (bottom < top)
		{
			Swap(ptr[bottom++], ptr[top--]);
		}
	}

} // namespace CE


