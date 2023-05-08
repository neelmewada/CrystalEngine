#pragma once

#if PLATFORM_WINDOWS
#include "PAL/Windows/VulkanWindowsPlatform.h"
#elif PLATFORM_MAC
#include "PAL/Mac/VulkanMacPlatform.h"
#endif

#include "PAL/SDL/VulkanSDLPlatform.h"


