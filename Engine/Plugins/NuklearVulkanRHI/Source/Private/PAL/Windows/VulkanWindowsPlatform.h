#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "vulkan/vulkan.h"
#include "vulkan/vulkan_win32.h"
#include "vma/vk_mem_alloc.h"

namespace CE
{
    
    class VulkanWindowsPlatform : public VulkanAPIPlatform
    {
        CE_STATIC_CLASS(VulkanWindowsPlatform)
    public:

        static CE::Array<const char*> GetRequiredInstanceExtensions()
        {
            CE::Array<const char*> ext = VulkanAPIPlatform::GetRequiredInstanceExtensions();
            
            ext.AddRange({
                VK_KHR_SURFACE_EXTENSION_NAME,
                VK_KHR_WIN32_SURFACE_EXTENSION_NAME
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

            VkWin32SurfaceCreateInfoKHR surfaceCI{};
            surfaceCI.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
            surfaceCI.hinstance = GetModuleHandle(NULL);
            surfaceCI.hwnd = (HWND)outWindowInfo.nativeWindowHandle;
            
            auto func = (PFN_vkCreateWin32SurfaceKHR)vkGetInstanceProcAddr(vkInstance, "vkCreateWin32SurfaceKHR");
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

    typedef VulkanWindowsPlatform VulkanPlatform;
    
} // namespace CE
