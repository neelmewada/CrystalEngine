#pragma once

#if PLATFORM_WINDOWS
#include "PAL/Windows/WindowsProcess.h"
#elif PLATFORM_MAC
#include "PAL/Mac/MacProcess.h"
#elif PLATFORM_LINUX
#include "PAL/Linux/LinuxProcess.h"
#endif
