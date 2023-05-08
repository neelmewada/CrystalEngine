#pragma once

struct VkInstance_T;
typedef VkInstance_T* VkInstance;

struct VkSurfaceKHR_T;
typedef VkSurfaceKHR_T* VkSurfaceKHR;

namespace CE
{
    
    class COREAPPLICATION_API PlatformWindow
    {
    protected:
        PlatformWindow();

    public:

        virtual ~PlatformWindow();


        virtual void* GetUnderlyingHandle() = 0;

        virtual void Show() = 0;
        virtual void Hide() = 0;

        virtual void GetWindowSize(u32* outWidth, u32* outHeight) = 0;
        virtual void GetDrawableWindowSize(u32* outWidth, u32* outHeight) = 0;

        virtual VkSurfaceKHR CreateVulkanSurface(VkInstance instance) = 0;

    protected:
    };

} // namespace CE
