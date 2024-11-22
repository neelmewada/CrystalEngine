#pragma once

#include <vulkan/vulkan.h>

namespace CE::Vulkan
{
    class Texture;
    class Sampler;

    class VulkanPlatformBase
    {
        CE_STATIC_CLASS(VulkanPlatformBase)
    public:
        
        
    };
}

#if PLATFORM_WINDOWS
#include "PAL/Windows/VulkanWindowsPlatform.h"
#elif PLATFORM_MAC
#include "PAL/Mac/VulkanMacPlatform.h"
#elif PLATFORM_LINUX
#include "PAL/Linux/VulkanLinuxPlatform.h"
#endif

#include "PAL/SDL/VulkanSDLPlatform.h"


