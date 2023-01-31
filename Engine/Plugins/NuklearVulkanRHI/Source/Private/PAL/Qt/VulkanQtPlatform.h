#pragma once

#include "CoreMinimal.h"

#include "vulkan/vulkan.h"
#include "vma/vk_mem_alloc.h"

namespace CE
{

    class VulkanQtPlatform
    {
        CE_STATIC_CLASS(VulkanQtPlatform)
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
    };

    typedef VulkanQtPlatform VulkanPlatform;
    
} // namespace CE
