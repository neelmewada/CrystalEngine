#pragma once

struct VkInstance_T;
typedef VkInstance_T* VkInstance;

struct VkSurfaceKHR_T;
typedef VkSurfaceKHR_T* VkSurfaceKHR;

namespace CE
{
	typedef void* WindowHandle;
	class PlatformWindow;

	class IWindowCallbacks
	{
	public:

		virtual void OnWindowResized(PlatformWindow* window, u32 newWidth, u32 newHeight) {}

		virtual void OnWindowDestroyed(PlatformWindow* window) {}

	};
    
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

        virtual u32 GetWindowId() = 0;

		inline void AddListener(IWindowCallbacks* listener) { windowCallbacks.Add(listener); }

		inline void RemoveListener(IWindowCallbacks* listener) { windowCallbacks.Remove(listener); }

    protected:

		List<IWindowCallbacks*> windowCallbacks{};

		friend class PlatformApplication;
    };

} // namespace CE
