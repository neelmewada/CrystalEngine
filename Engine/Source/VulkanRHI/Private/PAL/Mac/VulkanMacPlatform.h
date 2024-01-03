#pragma once

#include "vulkan/vulkan.h"
#include "vulkan/vulkan_macos.h"
#include "vma/vk_mem_alloc.h"

namespace CE::Vulkan
{
    class VulkanMacPlatform : public VulkanPlatformBase
    {
        CE_STATIC_CLASS(VulkanMacPlatform)
    public:
        
        static VkInstanceCreateFlags GetVulkanInstanceFlags()
        {
            return VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
        }

        static CE::Array<const char*> GetRequiredInstanceExtensions()
        {
            return {
                VK_KHR_SURFACE_EXTENSION_NAME,
                VK_MVK_MACOS_SURFACE_EXTENSION_NAME,
                VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME
            };
        }

        static CE::Array<const char*> GetRequiredVulkanDeviceExtensions()
        {
            return
            {
                VK_KHR_SWAPCHAIN_EXTENSION_NAME
            };
        }

        static void GetNativeWindowSize(void* nativeWindowHandle, u32* width, u32* height)
        {
            
        }
    };
    
    typedef VulkanMacPlatform VulkanOSPlatform;

} // namespace CE
