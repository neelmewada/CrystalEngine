#pragma once

#if PLATFORM_MAC

#include "Misc/CoreDefines.h"
#include "IO/Path.h"

namespace CE
{
    
    class CORE_API MacDirectories
    {
    public:
        CE_STATIC_CLASS(MacDirectories);

        static IO::Path GetLaunchDir();

        static IO::Path GetModuleDir();

        static IO::Path GetLogDir();
    };

    typedef MacDirectories PlatformDirectories;

} // namespace CE


#endif
