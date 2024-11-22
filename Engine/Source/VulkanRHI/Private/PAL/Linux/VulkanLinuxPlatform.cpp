#include "PAL/Common/VulkanPlatform.h"

#include <X11/Xlib.h>

#include "vulkan/vulkan_xlib.h"
#include "vulkan/vulkan_wayland.h"
#include "vma/vk_mem_alloc.h"

namespace CE::Vulkan
{
    CE::Array<const char*> VulkanLinuxPlatform::GetRequiredInstanceExtensions()
    {
        return
        {
            VK_KHR_SURFACE_EXTENSION_NAME,
            VK_KHR_XLIB_SURFACE_EXTENSION_NAME,
            VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME
        };
    }

}
