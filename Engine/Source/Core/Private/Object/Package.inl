#pragma once

#define PACKAGE_MAGIC_NUMBER CE::FromBigEndian((u64)0x005041434b00000a) // .PACK..\n

#define PACKAGE_VERSION_MAJOR (u32)1
#define PACKAGE_VERSION_MINOR (u32)2

#define PACKAGE_OBJECT_MAGIC_NUMBER CE::FromBigEndian((u64)0x004f424a45435400) // .OBJECT.

#define FIELD_MAGIC_NUMBER CE::FromBigEndian((u32)0xf11ff11f)

namespace CE
{
	enum PackageFeaturesVersion
	{
		PackageDependencies_Major = 1,
		PackageDependencies_Minor = 1,

		// Field entry magic value entry
		FieldMagicValue_Major = 1,
		FieldMagicValue_Minor = 2,
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
