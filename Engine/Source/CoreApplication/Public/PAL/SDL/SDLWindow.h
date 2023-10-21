#pragma once

struct SDL_Window;

namespace CE
{

    class COREAPPLICATION_API SDLPlatformWindow : public PlatformWindow
    {
    public:

        ~SDLPlatformWindow();

        virtual void* GetUnderlyingHandle() override;
		virtual void* GetOSNativeHandle() override;

        void Show() override;

        void Hide() override;

		bool IsFocussed() override;

        virtual void GetWindowSize(u32* outWidth, u32* outHeight) override;
        virtual void GetDrawableWindowSize(u32* outWidth, u32* outHeight) override;

		virtual void SetResizable(bool resizable) override;

        VkSurfaceKHR CreateVulkanSurface(VkInstance instance) override;

        u32 GetWindowId() override;

    protected:

        friend class SDLApplication;
        
        SDLPlatformWindow(const String& title, u32 width, u32 height, bool maximised, bool fullscreen, bool resizable = true);

        SDL_Window* handle = nullptr;

    };

    
} // namespace CE
