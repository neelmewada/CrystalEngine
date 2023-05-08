#pragma once

#include "SDL.h"
#include "SDL_vulkan.h"

#include "vulkan/vulkan.h"
#include "vma/vk_mem_alloc.h"

namespace CE
{
    
    class VulkanSDLPlatform : public VulkanOSPlatform
    {
        
    public:

        static bool IsValidationEnabled()
        {
#if CE_BUILD_DEBUG
            return true;
#else
            return false;
#endif
        }

        static bool IsVerboseValidationEnabled()
        {
#if CE_BUILD_DEBUG
            return true;
#else
            return false;
#endif
        }

        static Array<const char*> GetRequiredInstanceExtensions()
        {
            auto extensions = VulkanOSPlatform::GetRequiredInstanceExtensions();
#if CE_BUILD_DEBUG
            extensions.AddRange({
                VK_EXT_DEBUG_UTILS_EXTENSION_NAME
            });
#else
            
#endif
            
            return extensions;
        }

        static Array<const char*> GetValidationLayers()
        {
            return
            {
                "VK_LAYER_KHRONOS_validation"
            };
        }

        static VkSurfaceKHR CreateSurface(VkInstance vkInstance, void* windowHandle)
        {
            VkSurfaceKHR surface = nullptr;
            SDL_Vulkan_CreateSurface((SDL_Window*)windowHandle, vkInstance, &surface);
            return surface;
        }

        static void DestroySurface(VkInstance vkInstance, VkSurfaceKHR surface)
        {
            vkDestroySurfaceKHR(vkInstance, surface, nullptr);
        }

        static void GetDrawableWindowSize(void* sdlWindow, u32* width, u32* height)
        {
            int w = 0, h = 0;
            SDL_Vulkan_GetDrawableSize((SDL_Window*)sdlWindow, &w, &h);
            *width = (u32)w;
            *height = (u32)h;
        }
    };

    typedef VulkanSDLPlatform VulkanPlatform;

} // namespace CE
