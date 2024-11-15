#pragma once

#if PLATFORM_WINDOWS
#include "PAL/Windows/WindowsTLS.h"
#elif PLATFORM_MAC
#include "PAL/Mac/MacTLS.h"
#elif PLATFORM_LINUX
#include "PAL/Linux/LinuxTLS.h"
#endif
