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
                VK_MVK_MACOS_SURFACE_EXTENSION_NAME
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

        static VkSurfaceKHR CreateTestSurface(VkInstance vkInstance, VulkanTestWindow& outWindowInfo)
        {
            outWindowInfo = CreateTestVulkanWindow(vkInstance);
            
            VkMacOSSurfaceCreateInfoMVK surfaceCI{};
            surfaceCI.sType = VK_STRUCTURE_TYPE_MACOS_SURFACE_CREATE_INFO_MVK;
            surfaceCI.pNext = nullptr;
            surfaceCI.pView = outWindowInfo.nativeWindowHandle;
            
            auto func = (PFN_vkCreateMacOSSurfaceMVK)vkGetInstanceProcAddr(vkInstance, "vkCreateMacOSSurfaceMVK");
            if (func == nullptr)
            {
                DestroyTestVulkanWindow(outWindowInfo);
                return nullptr;
            }
            
            VkSurfaceKHR surface = nullptr;
            if (func(vkInstance, &surfaceCI, nullptr, &surface) != VK_SUCCESS)
            {
                return nullptr;
            }

            return surface;
        }

        static void DestroyTestSurface(VkInstance vkInstance, VkSurfaceKHR testSurface, VulkanTestWindow& windowInfo)
        {
            if (testSurface != nullptr)
            {
                vkDestroySurfaceKHR(vkInstance, testSurface, nullptr);
            }

            DestroyTestVulkanWindow(windowInfo);
        }
    };
    
    typedef VulkanMacPlatform VulkanPlatform;

} // namespace CE
