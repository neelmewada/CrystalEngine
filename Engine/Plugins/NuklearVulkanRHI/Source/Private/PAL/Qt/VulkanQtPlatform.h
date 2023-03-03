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
    struct VulkanTestWindow
    {
        void* qtWindowHandle;
        void* nativeWindowHandle;
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

        //static VulkanTestSurface CreateTestSurface(VkInstance vkInstance)
        //{
        //    VulkanTestSurface data{};

        //    QVulkanInstance* instance = new QVulkanInstance;
        //    instance->setVkInstance(vkInstance);

        //    QVulkanWindow* testWindow = new QVulkanWindow;
        //    testWindow->setSurfaceType(::QSurface::VulkanSurface);
        //    testWindow->setVulkanInstance(instance);

        //    testWindow->resize(1024, 768);
        //    testWindow->show();

        //    auto testSurface = instance->surfaceForWindow(testWindow);

        //    data.instance = instance;
        //    data.testWindow = testWindow;
        //    data.testSurface = testSurface;

        //    return data;
        //}

        //static void DestroyTestSurface(VulkanTestSurface& data, VkInstance vkInstance)
        //{
        //    data.testWindow->hide();

        //    vkDestroySurfaceKHR(vkInstance, data.testSurface, nullptr);
        //    delete data.testWindow;
        //    delete data.instance;

        //    data.testSurface = nullptr;
        //    data.testWindow = nullptr;
        //    data.instance = nullptr;
        //}

        static VulkanTestWindow CreateTestVulkanWindow(VkInstance vkInstance)
        {
            if (vkInstance == nullptr)
                return { nullptr, nullptr };

            QMainWindow* testWindow = new QMainWindow;
            testWindow->show();
            return { testWindow, (void*)testWindow->winId() };
        }

        static void DestroyTestVulkanWindow(VulkanTestWindow& window)
        {
            if (window.qtWindowHandle == nullptr)
                return;

            QMainWindow* testWindow = (QMainWindow*)window.qtWindowHandle;
            testWindow->hide();
            delete testWindow;

            window.qtWindowHandle = nullptr;
            window.nativeWindowHandle = nullptr;
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
