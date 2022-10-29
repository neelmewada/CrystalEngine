
#include "PAL/Windows/WindowsDirectories.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#define MAX_PATH 512

namespace CE
{

    IO::Path WindowsDirectories::GetLaunchDir()
    {
        static char buffer[MAX_PATH];
        GetModuleFileNameA(NULL, buffer, MAX_PATH);
        return IO::Path(String(buffer)).GetParentPath();
    }

    IO::Path WindowsDirectories::GetModuleDir()
    {
        return GetLaunchDir();
    }

    IO::Path WindowsDirectories::GetLogDir()
    {
        return GetLaunchDir() / "Logs";
    }

}


