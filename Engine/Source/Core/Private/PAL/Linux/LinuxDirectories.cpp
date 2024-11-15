
#include "PAL/Linux/LinuxDirectories.h"

#include <limits.h>

namespace CE
{

    IO::Path LinuxDirectories::GetLaunchDir()
    {
        static char exe_path[PATH_MAX];
        ssize_t len = readlink("/proc/self/exe", exe_path, sizeof(exe_path) - 1);
        exe_path[len] = '\0';  // Null-terminate the string
        return IO::Path(String(exe_path)).GetParentPath();
    }

    IO::Path LinuxDirectories::GetModuleDir()
    {
        return GetLaunchDir();
    }

    IO::Path LinuxDirectories::GetLogDir()
    {
        return GetLaunchDir() / "Logs";
    }

    IO::Path LinuxDirectories::GetAppRootDir()
    {
        return GetLaunchDir();
    }

    IO::Path LinuxDirectories::GetEngineRootDir()
    {
        return GetLaunchDir();
    }

    IO::Path LinuxDirectories::GetGameRootDir()
    {
        return GetLaunchDir();
    }

    IO::Path LinuxDirectories::GetEngineDir()
    {
        return GetAppRootDir() / "Engine";
    }

    IO::Path LinuxDirectories::GetEditorDir()
    {
        return GetAppRootDir() / "Editor";
    }

    IO::Path LinuxDirectories::GetGameDir()
    {
#if PAL_TRAIT_BUILD_EDITOR
		ASSERT(!gProjectPath.IsEmpty(), "GetGameDir() called while project path is NOT set!");
		return gProjectPath / "Game";
#else
        return GetAppRootDir() / "Game";
#endif
    }

    IO::Path LinuxDirectories::GetAppDataDir()
    {
        return IO::Path("~/.local/share") / (gProjectName.IsEmpty() ? "CrystalEngine" : gProjectName);
    }

#if PAL_TRAIT_BUILD_EDITOR
    IO::Path LinuxDirectories::GetEditorAppDataDir()
    {
        return IO::Path("~/.local/share") / "CrystalEngine";
    }
#endif
}


