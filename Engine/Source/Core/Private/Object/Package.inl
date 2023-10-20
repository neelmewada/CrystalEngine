#pragma once

#define PACKAGE_MAGIC_NUMBER CE::FromBigEndian((u64)0x005041434b00000a) // .PACK..\n

#define PACKAGE_VERSION_MAJOR (u32)2
#define PACKAGE_VERSION_MINOR (u32)0

#define PACKAGE_OBJECT_MAGIC_NUMBER CE::FromBigEndian((u64)0x004f424a45435400) // .OBJECT.

namespace CE
{
	enum PackageFeaturesVersion
	{
		
	};

	static inline bool IsVersionGreaterThanOrEqualTo(u32 currentMajor, u32 currentMinor, u32 checkMajor, u32 checkMinor)
	{
		return currentMajor > checkMajor || (currentMajor == checkMajor && currentMinor >= checkMinor);
	}

	static inline bool IsCurrentVersionGreaterThanOrEqualTo(u32 checkMajor, u32 checkMinor)
	{
		return IsVersionGreaterThanOrEqualTo(PACKAGE_VERSION_MAJOR, PACKAGE_VERSION_MINOR, checkMajor, checkMinor);
	}
}
