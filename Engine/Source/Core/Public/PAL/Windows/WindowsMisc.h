#pragma once

#if PLATFORM_WINDOWS

#include "CoreTypes.h"
#include "IO/Path.h"

namespace CE
{
    
    class CORE_API WindowsMisc
    {
    public:
        CE_STATIC_CLASS(WindowsMisc);

        inline static String GetPlatformName()
        {
            return "Windows";
        }
    };

    typedef WindowsMisc PlatformMisc;

} // namespace CE


#endif
