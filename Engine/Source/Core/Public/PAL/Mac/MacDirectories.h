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

        static IO::Path GetAppRootDir();
        
        // Returns the root directory of the engine installation
        static IO::Path GetEngineRootDir();

        // Returns the root directory of the game installation
        static IO::Path GetGameRootDir();

        static IO::Path GetEngineDir();
        static IO::Path GetEditorDir();
        static IO::Path GetGameDir();

        static IO::Path GetModuleDir();

        static IO::Path GetLogDir();

        static IO::Path GetAppDataDir();
        
#if PAL_TRAIT_BUILD_EDITOR
        static IO::Path GetEditorAppDataDir();
#endif
    };

    typedef MacDirectories PlatformDirectories;

} // namespace CE


#endif
