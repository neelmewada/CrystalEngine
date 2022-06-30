#pragma once

#include <string>

namespace Vox::IO
{

// Returns the executable directory
const std::string GetBinDirectory();

// Returns a relative path to the directory containing resources.
// macOS: APP_NAME.app/Contents/Resources
// Linux: APP_DIRECTORY/share
// Win32: APP_DIRECTORY/shared
const std::string GetSharedDirectory();

}

