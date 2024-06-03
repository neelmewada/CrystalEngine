#pragma once

#define BUNDLE_MAGIC_NUMBER CE::FromBigEndian((u64)0x0042554e444c4500) // .BUNDLE.

#define BUNDLE_VERSION_MAJOR (u32)2
#define BUNDLE_VERSION_MINOR (u32)0

#define BUNDLE_OBJECT_MAGIC_NUMBER CE::FromBigEndian((u64)0x004f424a45435400) // .OBJECT.

namespace CE
{
	

	static inline bool IsVersionGreaterThanOrEqualTo(u32 currentMajor, u32 currentMinor, u32 checkMajor, u32 checkMinor)
	{
		return currentMajor > checkMajor || (currentMajor == checkMajor && currentMinor >= checkMinor);
	}

	static inline bool IsCurrentVersionGreaterThanOrEqualTo(u32 checkMajor, u32 checkMinor)
	{
		return IsVersionGreaterThanOrEqualTo(BUNDLE_VERSION_MAJOR, BUNDLE_VERSION_MINOR, checkMajor, checkMinor);
	}
}
