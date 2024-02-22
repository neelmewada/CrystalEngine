#pragma once

#if PLATFORM_WINDOWS

#include "CoreTypes.h"
#include "IO/Path.h"

namespace CE
{
    
    class CORE_API WindowsMisc : public PlatformMiscBase
    {
    public:
        CE_STATIC_CLASS(WindowsMisc);

        inline static String GetPlatformName()
        {
            return "Windows";
        }

        inline static PlatformName GetCurrentPlatform()
        {
            return PlatformName::Windows;
        }

        /*
         * Display name for the OS provided file manager.
         *  Windows: Explorer
         *  Mac: Finder
         *  Linux: File Manager
         */
        inline static String GetSystemFileExplorerDisplayName()
        {
            return "Explorer";
        }
    };

    typedef WindowsMisc PlatformMisc;

} // namespace CE


#endif
