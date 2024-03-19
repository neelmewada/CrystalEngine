#pragma once

struct SDL_Window;

namespace CE
{

    class COREAPPLICATION_API SDLPlatformWindow : public PlatformWindow
    {
    public:

        ~SDLPlatformWindow();

        virtual void* GetUnderlyingHandle() override;
		virtual WindowHandle GetOSNativeHandle() override;
        virtual String GetTitle() override;

        void Show() override;

        void Hide() override;

		bool IsFocussed() override;

        virtual void GetWindowSize(u32* outWidth, u32* outHeight) override;
        virtual void GetDrawableWindowSize(u32* outWidth, u32* outHeight) override;
        virtual Vec2i GetDrawableWindowSize() override;

		virtual void SetResizable(bool resizable) override;

        virtual void SetBorderless(bool borderless) override;

        virtual bool IsBorderless() override;
        virtual bool IsMinimized() override;
        virtual bool IsFullscreen() override;
        virtual bool IsShown() override;
        virtual bool IsHidden() override;

        VkSurfaceKHR CreateVulkanSurface(VkInstance instance) override;

        u64 GetWindowId() override;

    protected:

        friend class SDLApplication;
        
        SDLPlatformWindow(const String& title, u32 width, u32 height, bool maximised, bool fullscreen, bool resizable = true);

        SDL_Window* handle = nullptr;
    };

    
} // namespace CE
