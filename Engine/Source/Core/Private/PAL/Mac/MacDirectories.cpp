
#include "PAL/Mac/MacDirectories.h"

#include <mach-o/dyld.h>
#include <CoreServices/CoreServices.h>
#include <limits.h>

namespace CE
{

    IO::Path MacDirectories::GetLaunchDir()
    {
        static char buffer[PATH_MAX];
        uint32_t bufferSize = PATH_MAX;
        _NSGetExecutablePath(buffer, &bufferSize);
        return IO::Path(String(buffer)).GetParentPath();
    }

    IO::Path MacDirectories::GetModuleDir()
    {
        return GetLaunchDir();
    }

    IO::Path MacDirectories::GetLogDir()
    {
        return GetLaunchDir() / "Logs";
    }

    IO::Path MacDirectories::GetAppRootDir()
    {
        return GetLaunchDir();
    }

    IO::Path MacDirectories::GetEngineRootDir()
    {
        return GetLaunchDir();
    }

    IO::Path MacDirectories::GetGameRootDir()
    {
        return GetLaunchDir();
    }

    IO::Path MacDirectories::GetEngineDir()
    {
        return GetAppRootDir() / "Engine";
    }

    IO::Path MacDirectories::GetEditorDir()
    {
        return GetAppRootDir() / "Editor";
    }

    IO::Path MacDirectories::GetGameDir()
    {
#if PAL_TRAIT_BUILD_EDITOR
		ASSERT(!gProjectPath.IsEmpty(), "GetGameDir() called while project path is NOT set!");
		return gProjectPath;
#else
        return GetAppRootDir() / "Game";
#endif
    }

    IO::Path MacDirectories::GetAppDataDir()
    {
        FSRef ref;
        OSType folderType = kApplicationSupportFolderType;
        char path[PATH_MAX];
        
        FSFindFolder( kUserDomain, folderType, kCreateFolder, &ref );

        FSRefMakePath( &ref, (UInt8*)&path, PATH_MAX );
        
        return IO::Path(path) / (gProjectName.IsEmpty() ? "CrystalEngine" : gProjectName);
    }

#if PAL_TRAIT_BUILD_EDITOR
    IO::Path MacDirectories::GetEditorAppDataDir()
    {
        FSRef ref;
        OSType folderType = kApplicationSupportFolderType;
        char path[PATH_MAX];
        
        FSFindFolder( kUserDomain, folderType, kCreateFolder, &ref );

        FSRefMakePath( &ref, (UInt8*)&path, PATH_MAX );
        
        return IO::Path(path) / "CrystalEngine";
    }
#endif
}


