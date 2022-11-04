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
    };

    typedef MacProcess PlatformProcess;

} // namespace CE


#endif
