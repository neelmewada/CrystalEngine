
#include "PAL/Common/VulkanPlatform.h"

#include "vulkan/vulkan.h"
#include "vulkan/vulkan_macos.h"

#include <Cocoa/Cocoa.h>
#include <MetalKit/MetalKit.h>

namespace CE
{
    VkSurfaceKHR VulkanMacPlatform::CreateTestSurface(VkInstance vkInstance, VulkanTestWindow** outTestWindow)
    {
        *outTestWindow = CreateTestVulkanWindow(vkInstance);
        
        VkMacOSSurfaceCreateInfoMVK surfaceCI{};
        surfaceCI.sType = VK_STRUCTURE_TYPE_MACOS_SURFACE_CREATE_INFO_MVK;
        surfaceCI.pNext = nullptr;
        surfaceCI.pView = (NSView*)(*outTestWindow)->winId();
        
        auto func = (PFN_vkCreateMacOSSurfaceMVK)vkGetInstanceProcAddr(vkInstance, "vkCreateMacOSSurfaceMVK");
        if (func == nullptr)
        {
            DestroyTestVulkanWindow(*outTestWindow);
            return nullptr;
        }
        
        VkSurfaceKHR surface = nullptr;
        if (func(vkInstance, &surfaceCI, nullptr, &surface) != VK_SUCCESS)
        {
            return nullptr;
        }

        return surface;
    }

    VkSurfaceKHR VulkanMacPlatform::CreateSurface(VkInstance vkInstance, void* windowHandle)
    {
        NSView* view = (NSView*)windowHandle;
        
        if (![view.layer isKindOfClass:[CAMetalLayer class]])
        {
            [view setLayer:[CAMetalLayer layer]];
            [view setWantsLayer:YES];
        }
        
        VkMacOSSurfaceCreateInfoMVK surfaceCI{};
        surfaceCI.sType = VK_STRUCTURE_TYPE_MACOS_SURFACE_CREATE_INFO_MVK;
        surfaceCI.pView = view;
        
        auto func = (PFN_vkCreateMacOSSurfaceMVK)vkGetInstanceProcAddr(vkInstance, "vkCreateMacOSSurfaceMVK");
        if (func == nullptr)
        {
            return nullptr;
        }
        
        VkSurfaceKHR surface = nullptr;
        if (func(vkInstance, &surfaceCI, nullptr, &surface) != VK_SUCCESS)
        {
            return nullptr;
        }
        
        return surface;
    }

}
