#pragma once

#include "vulkan/vulkan.h"
#include "vulkan/vulkan_macos.h"
#include "vma/vk_mem_alloc.h"

namespace CE
{
    class VulkanMacPlatform : public VulkanAPIPlatform
    {
        CE_STATIC_CLASS(VulkanMacPlatform)
    public:

        static CE::Array<const char*> GetRequiredInstanceExtensions()
        {
            CE::Array<const char*> ext = VulkanAPIPlatform::GetRequiredInstanceExtensions();
            
            ext.AddRange({
                VK_KHR_SURFACE_EXTENSION_NAME,
                VK_MVK_MACOS_SURFACE_EXTENSION_NAME,
                VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME
            });

            return ext;
        }

        static CE::Array<const char*> GetRequiredVulkanDeviceExtensions()
        {
            return
            {
                VK_KHR_SWAPCHAIN_EXTENSION_NAME
            };
        }

        static CE::Array<const char*> GetValidationLayers()
        {
            CE::Array<const char*> layers = VulkanAPIPlatform::GetValidationLayers();
            return layers;
        }

        static VkSurfaceKHR CreateTestSurface(VkInstance vkInstance, VulkanTestWindow** outTestWindow);

        static void DestroyTestSurface(VkInstance vkInstance, VkSurfaceKHR testSurface, VulkanTestWindow* testWindow)
        {
            if (testSurface != nullptr)
            {
                vkDestroySurfaceKHR(vkInstance, testSurface, nullptr);
            }

            DestroyTestVulkanWindow(testWindow);
        }

        static VkSurfaceKHR CreateSurface(VkInstance vkInstance, void* windowHandle);
    };
    
    typedef VulkanMacPlatform VulkanPlatform;

} // namespace CE
