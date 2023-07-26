#pragma once

#include <vulkan/vulkan.h>

namespace CE
{
    class VulkanTexture;
    class VulkanSampler;

    class VulkanPlatformBase
    {
        CE_STATIC_CLASS(VulkanPlatformBase)
    public:
        
        static VkDescriptorSet AddImGuiTexture(VulkanTexture* texture, VulkanSampler* sampler);
        
        static void RemoveImGuiTexture(VkDescriptorSet imguiTexture);
        
    };
}

#if PLATFORM_WINDOWS
#include "PAL/Windows/VulkanWindowsPlatform.h"
#elif PLATFORM_MAC
#include "PAL/Mac/VulkanMacPlatform.h"
#endif

#include "PAL/SDL/VulkanSDLPlatform.h"


