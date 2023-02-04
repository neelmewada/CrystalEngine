#pragma once

#include "CoreMinimal.h"

#include "vulkan/vulkan.h"
#include "vma/vk_mem_alloc.h"

#include <QVulkanInstance>
#include <QApplication>
#include <QWidget>
#include <QWindow>

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

        static void* GetActiveWindowHandle()
        {
            if (qApp->activeWindow() == nullptr)
                return nullptr;
            return qApp->activeWindow()->windowHandle();
        }

        static VkSurfaceKHR CreateSurface(VkInstance vkInstance, void* windowHandle)
        {
            return QVulkanInstance::surfaceForWindow((QWindow*)windowHandle);
        }
    };

    typedef VulkanQtPlatform VulkanAPIPlatform;
    
} // namespace CE
