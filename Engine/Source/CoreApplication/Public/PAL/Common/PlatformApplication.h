#pragma once

#include "ApplicationMessageHandler.h"

namespace CE
{
    class PlatformWindow;

    typedef MultiCastDelegate<void(PlatformWindow*, u32, u32)> WindowResizeDelegate;

    enum class PlatformBackend
    {
        None = 0,
        SDL
    };
    
    class COREAPPLICATION_API PlatformApplication
    {
    public:

        static PlatformApplication* Get();

        PlatformApplication() = default;

        virtual ~PlatformApplication();

        virtual void AddMessageHandler(ApplicationMessageHandler* handler)
        {
            this->messageHandlers.Add(handler);
        }

        virtual void RemoveMessageHandler(ApplicationMessageHandler* handler)
        {
            this->messageHandlers.Remove(handler);
        }

        virtual const Array<ApplicationMessageHandler*>& GetMessageHandlers() const
        {
            return messageHandlers;
        }

        virtual void Initialize();
        virtual void PreShutdown();
        virtual void Shutdown();

        virtual void Tick();

        virtual PlatformBackend GetBackend() = 0;

        virtual PlatformWindow* InitMainWindow(const String& title, u32 width, u32 height, bool maximised, bool fullscreen, bool resizable = true) = 0;

        virtual PlatformWindow* GetMainWindow() = 0;

        virtual PlatformWindow* CreatePlatformWindow(const String& title) = 0;
        virtual PlatformWindow* CreatePlatformWindow(const String& title, u32 width, u32 height, bool maximised, bool fullscreen) = 0;

        virtual void DestroyWindow(PlatformWindow* window) = 0;

        WindowResizeDelegate onWindowResized{};

    protected:

        static PlatformApplication* instance;

        Array<ApplicationMessageHandler*> messageHandlers{};
    };

} // namespace CE
