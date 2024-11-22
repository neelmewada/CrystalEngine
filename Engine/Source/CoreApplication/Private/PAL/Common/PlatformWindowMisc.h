#pragma once

namespace CE
{
    
} // namespace CE

#if PLATFORM_WINDOWS
#include "PAL/Windows/WindowsWindowMisc.h"
#elif PLATFORM_LINUX
#include "PAL/Linux/LinuxWindowMisc.h"
#elif PLATFORM_MAC
#include "PAL/Mac/MacWindowMisc.h"
#endif
