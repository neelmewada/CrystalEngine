
#include "PAL/Windows/WindowsDirectories.h"

#include <Windows.h>
#include <ShlObj_core.h>

#define MAX_PATH 512

namespace CE
{

    IO::Path WindowsDirectories::GetLaunchDir()
    {
        static char buffer[MAX_PATH];
        GetModuleFileNameA(NULL, buffer, MAX_PATH);
        return IO::Path(String(buffer)).GetParentPath();
    }

    IO::Path WindowsDirectories::GetAppRootDir()
    {
        return GetLaunchDir();
    }

    IO::Path WindowsDirectories::GetEngineDir()
    {
        return GetAppRootDir() / "Engine";
    }

    IO::Path WindowsDirectories::GetEditorDir()
    {
        return GetAppRootDir() / "Editor";
    }

    IO::Path WindowsDirectories::GetGameDir()
    {
        return GetAppRootDir() / "Game";
    }

    IO::Path WindowsDirectories::GetModuleDir()
    {
        return GetLaunchDir();
    }

    IO::Path WindowsDirectories::GetLogDir()
    {
        return GetLaunchDir() / "Logs";
    }

    IO::Path WindowsDirectories::GetAppDataDir()
    {
        PWSTR path = NULL;
        SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, NULL, &path);

        IO::Path appDataPath{ path };

        appDataPath = appDataPath / "CrystalEngine";
        
        CoTaskMemFree(path);
        return appDataPath;
    }

}


