#pragma once

#if PLATFORM_LINUX

#include "CoreTypes.h"
#include "IO/Path.h"

namespace CE
{
    
    class CORE_API LinuxMisc : public PlatformMiscBase
    {
    public:

        inline static String GetPlatformName()
        {
            return "Linux";
        }

        inline static PlatformName GetCurrentPlatform()
        {
            return PlatformName::Linux;
        }
    };

    typedef LinuxMisc PlatformMisc;

} // namespace CE


#endif
