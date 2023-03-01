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
            auto windowList = qApp->topLevelWindows();
            if (windowList.size() == 0)
                return nullptr;
            return windowList.at(0);
        }

        static VkSurfaceKHR CreateSurface(VkInstance vkInstance, void* windowHandle)
        {
            QVulkanInstance* instance = new QVulkanInstance;
            instance->setVkInstance(vkInstance);

            auto surface = instance->surfaceForWindow((QWindow*)windowHandle);

            delete instance;
            return surface;
        }
    };

    typedef VulkanQtPlatform VulkanAPIPlatform;
    
} // namespace CE