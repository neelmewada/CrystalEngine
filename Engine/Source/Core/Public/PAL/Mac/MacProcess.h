#pragma once

#if PLATFORM_MAC

#include "CoreTypes.h"
#include "IO/Path.h"

namespace CE
{
    
    class CORE_API MacProcess
    {
    public:
        CE_STATIC_CLASS(MacProcess);

        static void* LoadDll(IO::Path path);

        static void* GetDllSymbol(void* dllHandle, const char* procName);

        static void UnloadDll(void* dllHandle);

        static IO::Path GetModuleDllPath(String moduleName);

        static String GetDllDecoratedName(String baseName);
        
        static bool LaunchProcess(const IO::Path& executablePath, const String& args);
    };

    typedef MacProcess PlatformProcess;

} // namespace CE


#endif
