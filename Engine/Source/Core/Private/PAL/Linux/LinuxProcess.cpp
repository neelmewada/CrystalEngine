
#include "PAL/Linux/LinuxProcess.h"
#include "PAL/Linux/LinuxDirectories.h"

#include <dlfcn.h>

namespace CE
{

	void* LinuxProcess::LoadDll(IO::Path path)
	{
		String pathString = path.GetString();
        
        auto ptr = dlopen(pathString.GetCString(), RTLD_LOCAL | RTLD_LAZY);
        return ptr;
	}

	void* LinuxProcess::GetDllSymbol(void* dllHandle, const char* procName)
	{
        return dlsym(dllHandle, procName);
	}

	void LinuxProcess::UnloadDll(void* dllHandle)
	{
        dlclose(dllHandle);
	}

	IO::Path LinuxProcess::GetModuleDllPath(String moduleName)
	{
		String dllName = "lib" + moduleName + ".so";
		return PlatformDirectories::GetLaunchDir() / dllName;
	}

	String LinuxProcess::GetDllDecoratedName(String baseName)
	{
		return "lib" + baseName + ".dylib";
	}

    bool LinuxProcess::LaunchProcess(const IO::Path& executablePath, const String& args)
    {
		String command = executablePath.GetString() + " \"" + args + "\" &";
        system(command.GetCString());
        return true;
    }

}
