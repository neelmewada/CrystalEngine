#pragma once

namespace CE
{

    enum class PlatformName
    {
        None = 0,
        Windows,
        Mac,
        Linux,
        Android,
        iOS
    };
    ENUM_CLASS(PlatformName);

    enum class PlatformFlags
    {
        None = 0,
        Windows = BIT((int)PlatformName::Windows),
        Mac		= BIT((int)PlatformName::Mac),
        Linux	= BIT((int)PlatformName::Linux),
        Android = BIT((int)PlatformName::Android),
        iOS		= BIT((int)PlatformName::iOS),

        Desktop = Windows | Mac | Linux,
        Mobile = Android | iOS,
    };
    ENUM_CLASS_FLAGS(PlatformFlags);

    
} // namespace CE
