#pragma once

namespace CE
{
    class VulkanTexture;

    class VulkanPlatformBase
    {
        CE_STATIC_CLASS(VulkanPlatformBase)
    public:
        
        static VkDescriptorSet AddImGuiTexture(VulkanTexture* texture);
        
    };
}

#if PLATFORM_WINDOWS
#include "PAL/Windows/VulkanWindowsPlatform.h"
#elif PLATFORM_MAC
#include "PAL/Mac/VulkanMacPlatform.h"
#endif

#include "PAL/SDL/VulkanSDLPlatform.h"


