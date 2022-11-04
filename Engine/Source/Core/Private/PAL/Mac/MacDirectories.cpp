
#include "PAL/Mac/MacDirectories.h"

#include <mach-o/dyld.h>
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

}


