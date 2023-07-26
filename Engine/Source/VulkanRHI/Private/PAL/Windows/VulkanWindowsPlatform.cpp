
#include "PAL/Common/VulkanPlatform.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "vulkan/vulkan_win32.h"
#include "vma/vk_mem_alloc.h"

namespace CE
{
    Array<const char*> VulkanWindowsPlatform::GetRequiredInstanceExtensions()
    {
        return 
        {
            VK_KHR_SURFACE_EXTENSION_NAME,
            VK_KHR_WIN32_SURFACE_EXTENSION_NAME
        };
    }

    void VulkanWindowsPlatform::GetNativeWindowSize(void* nativeWindowHandle, u32* width, u32* height)
	{
        HWND hWnd = (HWND)nativeWindowHandle;
        RECT rect;
        if (GetWindowRect(hWnd, &rect))
        {
            *width = rect.right - rect.left;
            *height = rect.bottom - rect.top;
        }
	}
}
