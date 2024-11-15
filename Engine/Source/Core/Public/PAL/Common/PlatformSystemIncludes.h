#pragma once

#if PLATFORM_WINDOWS
#include "PAL/Windows/WindowsSystemIncludes.h"
#elif PLATFORM_MAC
#include "PAL/Mac/MacSystemIncludes.h"
#elif PLATFORM_LINUX
#include "PAL/Linux/LinuxSystemIncludes.h"
#endif
