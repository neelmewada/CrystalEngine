#pragma once

union SDL_Event;

namespace CE
{
    class SDLPlatformWindow;

    class COREAPPLICATION_API SDLApplication : public PlatformApplication
    {
    public:
        typedef PlatformApplication Super;

        static SDLApplication* Create();

        static SDLApplication* Get();

        virtual ~SDLApplication();

        void Initialize() override;

        void PreShutdown() override;

        void Shutdown() override;

        PlatformBackend GetBackend() override
        {
            return PlatformBackend::SDL;
        }

        PlatformWindow* InitMainWindow(const String& title, u32 width, u32 height, bool maximised, bool fullscreen, bool resizable = true) override;

        PlatformWindow* GetMainWindow() override;

        virtual PlatformWindow* CreatePlatformWindow(const String& title) override;
        virtual PlatformWindow* CreatePlatformWindow(const String& title, u32 width, u32 height, bool maximised, bool fullscreen) override;

		virtual Vec2i GetMainScreenSize() override;
		virtual Vec2i GetScreenSizeForWindow(PlatformWindow* window) override;

		virtual Vec2i GetWindowSize(void* nativeWindowHandle) override;

        void DestroyWindow(PlatformWindow* window) override;

        void Tick() override;

    public:
        Mutex mutex{};

        bool mainWindowForceResizing = false;

    protected:

        void ProcessWindowEvents(SDL_Event& event);

        void ProcessWindowResizeEvent(SDLPlatformWindow* window);

        SDLApplication();

        SDLPlatformWindow* mainWindow = nullptr;

        Array<SDLPlatformWindow*> windowList{};
    };

    typedef SDLApplication PlatformApplicationImpl;
    
} // namespace CE
