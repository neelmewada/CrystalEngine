
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

	CORE_API SIZE_T GetCombinedHashes(CE::Array<SIZE_T> hashes)
	{
		if (hashes.GetSize() == 0)
			return 0;

		SIZE_T hash = hashes[0];

		for (int i = 1; i < hashes.GetSize(); i++)
		{
			hash = GetCombinedHash(hash, hashes[i]);
		}

		return hash;
	}

}
