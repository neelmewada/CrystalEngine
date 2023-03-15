#pragma once

#include "CoreMinimal.h"

#include "vulkan/vulkan.h"
#include "vma/vk_mem_alloc.h"

#include <QVulkanInstance>
#include <QVulkanWindow>
#include <QApplication>
#include <QWidget>
#include <QWindow>
#include <QMainWindow>

namespace CE
{
    class VulkanTestWindow : public QWindow
    {
    public:
        VulkanTestWindow(QWindow* parent = nullptr) : QWindow(parent)
        {
            setSurfaceType(SurfaceType::VulkanSurface);
        }
    };

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

        static VulkanTestWindow* CreateTestVulkanWindow(VkInstance vkInstance)
        {
            if (vkInstance == nullptr)
                return nullptr;
            
            auto window = new VulkanTestWindow;
            window->show();
            return window;
        }

        static void DestroyTestVulkanWindow(VulkanTestWindow* window)
        {
            delete window;
        }

        static VkSurfaceKHR CreateSurface(VkInstance vkInstance, void* windowHandle)
        {
            return nullptr;
        }
    };

    typedef VulkanQtPlatform VulkanAPIPlatform;
    
} // namespace CE
