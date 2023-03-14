#pragma once

#include "CoreMinimal.h"

#include "SDL.h"
#include "SDL_vulkan.h"

#include "vulkan/vulkan.h"
#include "vma/vk_mem_alloc.h"

namespace CE
{
    
    class VulkanSDLPlatform
    {
        CE_STATIC_CLASS(VulkanSDLPlatform)
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

        static CE::Array<const char*> GetRequiredInstanceExtensions()
        {
#if CE_BUILD_DEBUG
            return
            {
                VK_EXT_DEBUG_UTILS_EXTENSION_NAME
            };
#else
            return
            {

            };
#endif
        }

        static CE::Array<const char*> GetValidationLayers()
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
    };

    typedef VulkanSDLPlatform VulkanAPIPlatform;

} // namespace CE
