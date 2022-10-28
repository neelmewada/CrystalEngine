
#include "Algorithm/Hash.h"

#include "xxhash.h"

namespace CE
{

	SIZE_T CalculateHash(const void* data, SIZE_T length)
	{
#if IS_64BIT
		return XXH64(data, length, 0);
#else
		return XXH32(data, length, 0);
#endif
	}

}
