#pragma once

#if PLATFORM_WINDOWS
#   include "PAL/Windows/WindowsMemory.h"
#elif PLATFORM_MAC
#   include "PAL/Mac/MacMemory.h"
#endif
