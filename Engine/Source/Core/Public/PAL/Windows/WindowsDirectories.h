#pragma once

#if PLATFORM_WINDOWS

#include "Misc/CoreDefines.h"
#include "IO/Path.h"

namespace CE
{
    
    class CORE_API WindowsDirectories
    {
    public:
        CE_STATIC_CLASS(WindowsDirectories);

        static IO::Path GetLaunchDir();

        static IO::Path GetModuleDir();

        static IO::Path GetLogDir();
    };

    typedef WindowsDirectories PlatformDirectories;

} // namespace CE


#endif
