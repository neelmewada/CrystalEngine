
#include "IO/FileManager.h"

#include <filesystem>
#include <iostream>

#if PLATFORM_MACOS
#include <mach-o/dyld.h>
#endif

#if PLATFORM_WIN32
#include <Windows.h>
#endif

namespace fs = std::filesystem;

using namespace Vox::IO;

FileManager::FileManager()
{

}

FileManager::~FileManager()
{

}

const std::string FileManager::GetSharedDirectoryImpl()
{
    fs::path curPath = GetBinDirectoryImpl();
#if PLATFORM_MACOS
    return (curPath / "../Resources/").string();
#elif PLATFORM_LINUX
    return (curPath / "share").string();
#elif PLATFORM_WIN32
    return curPath.string();
#else
#   error Unsupported Platform
#endif
}

const std::string FileManager::GetBinDirectoryImpl()
{
    char path[1024];
    uint32_t size = sizeof(path);
#if PLATFORM_MACOS
    _NSGetExecutablePath(path, &size);
#elif PLATFORM_WIN32
    GetModuleFileName(NULL, path, size);
#elif PLATFORM_LINUX
    readlink("/proc/self/exe", result, size);
#endif
    fs::path dirPath = fs::path(std::string(path)).parent_path();
    return dirPath.string();
}
