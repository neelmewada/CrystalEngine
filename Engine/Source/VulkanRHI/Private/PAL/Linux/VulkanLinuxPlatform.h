#pragma once

#include "vulkan/vulkan.h"
//#include "vma/vk_mem_alloc.h"

namespace CE::Vulkan
{
    class VulkanLinuxPlatform : public VulkanPlatformBase
    {
    public:
        
        static VkInstanceCreateFlags GetVulkanInstanceFlags()
        {
            return VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
        }

        static CE::Array<const char*> GetRequiredInstanceExtensions();

        static CE::Array<const char*> GetRequiredVulkanDeviceExtensions()
        {
            return
            {
                VK_KHR_SWAPCHAIN_EXTENSION_NAME
            };
        }
    };
    
    typedef VulkanLinuxPlatform VulkanOSPlatform;

} // namespace CE
