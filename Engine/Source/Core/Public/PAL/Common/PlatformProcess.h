#pragma once

#if PLATFORM_WINDOWS
#include "PAL/Windows/WindowsProcess.h"
#elif PLATFORM_MAC
#include "PAL/Mac/MacProcess.h"
#endif
