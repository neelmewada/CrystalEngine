#pragma once

namespace CE
{

    class COREAPPLICATION_API SDLApplication : public PlatformApplication
    {
    public:
        typedef PlatformApplication Super;

        static SDLApplication* Create();

        virtual ~SDLApplication();

        void Initialize() override;

        void PreShutdown() override;

        void Shutdown() override;

        PlatformWindow* InitMainWindow(const String& title, u32 width, u32 height, bool maximised, bool fullscreen) override;

        PlatformWindow* GetMainWindow() override;

        void Tick() override;

    protected:

        SDLApplication();

        PlatformWindow* mainWindow = nullptr;
    };

    typedef SDLApplication PlatformApplicationImpl;
    
} // namespace CE
