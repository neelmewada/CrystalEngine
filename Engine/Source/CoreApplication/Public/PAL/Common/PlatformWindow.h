#pragma once

struct VkInstance_T;
typedef VkInstance_T* VkInstance;

struct VkSurfaceKHR_T;
typedef VkSurfaceKHR_T* VkSurfaceKHR;

namespace CE
{
	typedef void* WindowHandle;
	class PlatformWindow;
    
    class COREAPPLICATION_API PlatformWindow
    {
    protected:
        PlatformWindow();

        virtual ~PlatformWindow();

    public:

        virtual void* GetUnderlyingHandle() = 0;

		virtual WindowHandle GetOSNativeHandle() = 0;

        virtual void Show() = 0;
        virtual void Hide() = 0;

		virtual bool IsFocussed() = 0;

        virtual void GetWindowSize(u32* outWidth, u32* outHeight) = 0;
        virtual void GetDrawableWindowSize(u32* outWidth, u32* outHeight) = 0;

        virtual VkSurfaceKHR CreateVulkanSurface(VkInstance instance) = 0;

		virtual void SetResizable(bool resizable) = 0;

        virtual void SetBorderless(bool borderless) = 0;

        virtual bool IsMinimized() = 0;
        virtual bool IsFullscreen() = 0;
        virtual bool IsShown() = 0;
        virtual bool IsHidden() = 0;

        virtual u64 GetWindowId() = 0;

        inline bool IsMainWindow() const { return isMainWindow; }

    protected:

		//List<IWindowCallbacks*> windowCallbacks{};

        bool isMainWindow = false;

		friend class PlatformApplication;
    };

} // namespace CE
