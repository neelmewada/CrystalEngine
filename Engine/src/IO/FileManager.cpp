
#include "IO/FileManager.h"

#include <filesystem>

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
    fs::path curPath = fs::current_path();
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
    return fs::current_path().string();
}
