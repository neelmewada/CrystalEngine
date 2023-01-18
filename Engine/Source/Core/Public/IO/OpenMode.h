#pragma once

namespace CE::IO
{
    
    enum class OpenMode : CE::u32
    {
        Invalid = 0,
        ModeRead = (1 << 0),
        ModeWrite = (1 << 1),
        ModeAppend = (1 << 2),
        ModeBinary = (1 << 3),
        ModeText = (1 << 4),
        ModeUpdate = (1 << 5),
        ModeCreatePath = (1 << 6),
    };
    ENUM_CLASS_FLAGS(OpenMode)

} // namespace CE::IO
