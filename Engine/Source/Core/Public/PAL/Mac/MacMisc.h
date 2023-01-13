#pragma once

#if PLATFORM_MAC

#include "CoreTypes.h"
#include "IO/Path.h"

namespace CE
{
    
    class CORE_API MacMisc
    {
    public:
        CE_STATIC_CLASS(MacMisc);

        inline static String GetPlatformName()
        {
            return "Mac";
        }
    };

    typedef MacMisc PlatformMisc;

} // namespace CE


#endif
