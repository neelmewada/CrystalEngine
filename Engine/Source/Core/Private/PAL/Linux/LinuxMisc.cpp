
#include "PAL/Linux/LinuxMisc.h"

#include <sys/utsname.h>

namespace CE
{

    String LinuxMisc::GetOSVersionString()
    {
        utsname info;
        uname(&info);

        return String::Format("{} [{}] (x86_64)", info.sysname, info.release);
    }

}

