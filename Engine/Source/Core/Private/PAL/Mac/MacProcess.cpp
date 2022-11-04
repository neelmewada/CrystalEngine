
#include "PAL/Mac/MacProcess.h"
#include "PAL/Mac/MacDirectories.h"

#include <dlfcn.h>

namespace CE
{

	void* MacProcess::LoadDll(IO::Path path)
	{
		String pathString = path.GetString();
        
        return dlopen(pathString.GetCString(), RTLD_NOLOAD | RTLD_LAZY | RTLD_LOCAL);
	}

	void* MacProcess::GetDllSymbol(void* dllHandle, const char* procName)
	{
        return dlsym(dllHandle, procName);
	}

	void MacProcess::UnloadDll(void* dllHandle)
	{
        dlclose(dllHandle);
	}

	IO::Path MacProcess::GetModuleDllPath(String moduleName)
	{
		String dllName = "lib" + moduleName + ".dylib";
		return PlatformDirectories::GetLaunchDir() / dllName;
	}

}
