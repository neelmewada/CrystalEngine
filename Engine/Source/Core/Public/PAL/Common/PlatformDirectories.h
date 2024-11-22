#pragma once

#if PLATFORM_WINDOWS
#include "PAL/Windows/WindowsDirectories.h"
#elif PLATFORM_MAC
#include "PAL/Mac/MacDirectories.h"
#elif PLATFORM_LINUX
#include "PAL/Linux/LinuxDirectories.h"
#endif
