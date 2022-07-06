#pragma once

#include "EngineDefs.h"

#include <string>

namespace Vox::IO
{

class ENGINE_API FileManager
{
private:
    FileManager();
    ~FileManager();
    FileManager(const FileManager&) = delete;

public:
    static FileManager* Get()
    {
        static FileManager fileManager;
        return &fileManager;
    }

public: // Public API
    // -- Getters --

    // Returns a relative path to the directory containing resources.
    // macOS: EXEC_NAME.app/Contents/Resources
    // Linux: EXEC_DIR/share
    // Win32: EXEC_DIR/shared
    static const std::string GetSharedDirectory() { return Get()->GetSharedDirectoryImpl(); }

    // Returns the executable directory
    static const std::string GetBinDirectory() { return Get()->GetBinDirectoryImpl(); }

private: // Internal API
    const std::string GetSharedDirectoryImpl();
    const std::string GetBinDirectoryImpl();

};

}
