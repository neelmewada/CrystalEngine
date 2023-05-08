#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "vulkan/vulkan.h"
#include "vulkan/vulkan_win32.h"
#include "vma/vk_mem_alloc.h"

namespace CE
{
    
    class VulkanWindowsPlatform
    {
        CE_STATIC_CLASS(VulkanWindowsPlatform)
    public:

        static VkInstanceCreateFlags GetVulkanInstanceFlags()
        {
            return 0;
        }

        static Array<const char*> GetRequiredInstanceExtensions()
        {
            return {
                VK_KHR_SURFACE_EXTENSION_NAME,
                VK_KHR_WIN32_SURFACE_EXTENSION_NAME
            };
        }

        static Array<const char*> GetRequiredVulkanDeviceExtensions()
        {
            return
            {
                VK_KHR_SWAPCHAIN_EXTENSION_NAME
            };
        }
        
        
        static void GetNativeWindowSize(void* nativeWindowHandle, u32* width, u32* height)
        {
            HWND hWnd = (HWND)nativeWindowHandle;
            RECT rect;
            if (GetWindowRect(hWnd, &rect))
            {
                *width = rect.right - rect.left;
                *height = rect.bottom - rect.top;
            }
        }
    };

    typedef VulkanWindowsPlatform VulkanOSPlatform;
    
} // namespace CE
