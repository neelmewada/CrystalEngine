#pragma once

#include "CoreMinimal.h"

#include "SDL.h"
#include "SDL_vulkan.h"

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

        static CE::Array<String> GetValidationLayers()
        {
            return
            {
                "VK_LAYER_KHRONOS_validation"
            };
        }
    };

    typedef VulkanSDLPlatform VulkanPlatform;

} // namespace CE
