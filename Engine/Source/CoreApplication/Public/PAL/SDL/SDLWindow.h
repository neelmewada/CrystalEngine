#pragma once

struct SDL_Window;

namespace CE
{

    class COREAPPLICATION_API SDLPlatformWindow : public PlatformWindow
    {
    public:

        ~SDLPlatformWindow();

        virtual void* GetUnderlyingHandle() override;

        void Show() override;

        void Hide() override;

        virtual void GetWindowSize(u32* outWidth, u32* outHeight) override;
        virtual void GetDrawableWindowSize(u32* outWidth, u32* outHeight) override;

    protected:

        friend class SDLApplication;
        
        SDLPlatformWindow(const String& title, u32 width, u32 height, bool maximised, bool fullscreen);

        SDL_Window* handle = nullptr;

    };

    
} // namespace CE
