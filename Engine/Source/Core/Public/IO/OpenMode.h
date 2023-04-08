#pragma once

namespace CE::IO
{
    
    enum class OpenMode : CE::u32
    {
        Invalid = 0,
        ModeRead = std::ios::in,
        ModeWrite = std::ios::out,
        ModeAppend = std::ios::app,
        ModeBinary = std::ios::binary,
    };
    ENUM_CLASS_FLAGS(OpenMode)

} // namespace CE::IO
