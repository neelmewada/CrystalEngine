#pragma once

#include "vulkan/vulkan.h"

namespace CE::Vulkan
{
    
    class VulkanWindowsPlatform : public VulkanPlatformBase
    {
        CE_STATIC_CLASS(VulkanWindowsPlatform)
    public:

        static VkInstanceCreateFlags GetVulkanInstanceFlags()
        {
            return 0;
        }

        static Array<const char*> GetRequiredInstanceExtensions();

        static Array<const char*> GetRequiredVulkanDeviceExtensions()
        {
            return
            {
                VK_KHR_SWAPCHAIN_EXTENSION_NAME
            };
        }
        
        
        static void GetNativeWindowSize(void* nativeWindowHandle, u32* width, u32* height);
    };

    typedef VulkanWindowsPlatform VulkanOSPlatform;
    
} // namespace CE
