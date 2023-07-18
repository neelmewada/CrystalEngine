
#include "PAL/Mac/MacProcess.h"
#include "PAL/Mac/MacDirectories.h"

#include <dlfcn.h>

namespace CE
{

	void* MacProcess::LoadDll(IO::Path path)
	{
		String pathString = path.GetString();
        
        auto ptr = dlopen(pathString.GetCString(), RTLD_LOCAL | RTLD_LAZY);
        return ptr;
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

	String MacProcess::GetDllDecoratedName(String baseName)
	{
		return "lib" + baseName + ".dylib";
	}

    bool MacProcess::LaunchProcess(const IO::Path& executablePath, const String& args)
    {
        String command = "\"" + executablePath.GetString() + "\" " + args + " &";
        system(command.GetCString());
        return true;
    }

}
