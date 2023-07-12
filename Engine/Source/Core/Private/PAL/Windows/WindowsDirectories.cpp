
#include "PAL/Windows/WindowsDirectories.h"

#include <Windows.h>
#include <ShlObj_core.h>

#define MAX_PATH_SIZE 512

namespace CE
{

    IO::Path WindowsDirectories::GetLaunchDir()
    {
        static char buffer[MAX_PATH_SIZE];
        GetModuleFileNameA(NULL, buffer, MAX_PATH_SIZE);
        return IO::Path(String(buffer)).GetParentPath();
    }

    IO::Path WindowsDirectories::GetAppRootDir()
    {
        return GetLaunchDir();
    }

    IO::Path WindowsDirectories::GetEngineRootDir()
    {
        return GetLaunchDir();
    }

    IO::Path WindowsDirectories::GetGameRootDir()
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
#if PAL_TRAIT_BUILD_EDITOR
		ASSERT(!gProjectPath.IsEmpty(), "GetGameDir() called while project path is NOT set!");
		return gProjectPath / "Game";
#else
        return GetAppRootDir() / "Game";
#endif
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

		appDataPath = appDataPath / (gProjectName.IsEmpty() ? "CrystalEngine" : gProjectName);
        
        CoTaskMemFree(path);
        return appDataPath;
    }

#if PAL_TRAIT_BUILD_EDITOR

	IO::Path WindowsDirectories::GetEditorAppDataDir()
	{
		PWSTR path = NULL;
		SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, NULL, &path);

		IO::Path appDataPath{ path };

		appDataPath = appDataPath / "CrystalEngine";

		CoTaskMemFree(path);
		return appDataPath;
	}

#endif

}


