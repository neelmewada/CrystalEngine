
#include "Algorithm/Hash.h"
#include "Containers/Array.h"

#include "xxhash.h"
#include "CRC.h"

namespace CE
{

	CORE_API SIZE_T CalculateHash(const void* data, SIZE_T length)
	{
#if IS_64BIT
		return XXH64(data, length, 0);
#else
		return XXH32(data, length, 0);
#endif
	}

	CORE_API u32 CalculateCRC(const void* data, SIZE_T size)
	{
		return CRC::Calculate(data, size, CRC::CRC_32());
	}

	CORE_API u32 CalculateCRC(const void* data, SIZE_T size, u32 crc)
	{
		return CRC::Calculate(data, size, CRC::CRC_32(), crc);
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
