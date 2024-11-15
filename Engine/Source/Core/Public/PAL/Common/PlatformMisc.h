#pragma once

namespace CE
{
	class CORE_API PlatformMiscBase
	{
		CE_STATIC_CLASS(PlatformMiscBase)
	public:

		inline static bool IsDesktopPlatform(PlatformName platform)
		{
			return platform == PlatformName::Windows || platform == PlatformName::Linux || platform == PlatformName::Mac;
		}

		inline static  bool IsMobilePlatform(PlatformName platform)
		{
			return platform == PlatformName::Android || platform == PlatformName::iOS;
		}

	};
}

#if PLATFORM_WINDOWS
#include "PAL/Windows/WindowsMisc.h"
#elif PLATFORM_MAC
#include "PAL/Mac/MacMisc.h"
#elif PLATFORM_LINUX
#include "PAL/Linux/LinuxMisc.h"
#endif
