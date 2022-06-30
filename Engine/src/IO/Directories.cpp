
#include "IO/Directories.h"
#include <filesystem>

namespace fs = std::filesystem;

namespace Vox::IO
{

const std::string GetBinDirectory()
{
    return fs::current_path().string();
}

const std::string GetSharedDirectory()
{
    fs::path curPath = fs::current_path();
#if PLATFORM_MACOS
    return (curPath / "../Resources/").string();
#elif PLATFORM_LINUX
    return (curPath / "share").string();
#else
    return (curPath / "shared").string();
#endif
}

}