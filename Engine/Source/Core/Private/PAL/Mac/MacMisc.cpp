
#include "PAL/Mac/MacMisc.h"

#include <CoreServices/CoreServices.h>

namespace CE
{
    static thread_local String osVersionString = "";

    String MacMisc::GetOSVersionString()
    {
        if (osVersionString.NotEmpty())
            return osVersionString;

        SInt32 majorVersion, minorVersion, bugFixVersion;

        Gestalt(gestaltSystemVersionMajor, &majorVersion);
        Gestalt(gestaltSystemVersionMinor, &minorVersion);
        Gestalt(gestaltSystemVersionBugFix, &bugFixVersion);

#if defined(__aarch64__)
        return String::Format("macOS {}.{}.{} (arm64)");
#elif defined(__x86_64__)
        return String::Format("macOS {}.{}.{} (x86_64)");
#else
        #error "Invalid architecture!"
#endif
    }

} // namespace CE

