#pragma once

#include "ApplicationMessageHandler.h"

namespace CE
{
    class PlatformWindow;

    typedef MultiCastDelegate<void(PlatformWindow* window, u32 newWidth, u32 newHeight)> WindowResizeDelegate;

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

        virtual void AddTickHandler(const Delegate<void(void)> tickHandler)
        {
            this->tickHanders.Add(tickHandler);
        }

        virtual void RemoveTickHandler(DelegateHandle tickHandle)
        {
            this->tickHanders.RemoveAll([&](const Delegate<void()>& del) { return del.GetHandle() == tickHandle; });
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

        virtual PlatformWindow* FindWindow(u64 windowId) = 0;

        virtual PlatformWindow* CreatePlatformWindow(const String& title) = 0;
        virtual PlatformWindow* CreatePlatformWindow(const String& title, u32 width, u32 height, bool maximised, bool fullscreen) = 0;

        virtual void DestroyWindow(PlatformWindow* window) = 0;

		virtual Vec2i GetMainScreenSize() = 0;
		virtual Vec2i GetScreenSizeForWindow(PlatformWindow* window) = 0;

		virtual Vec2i GetWindowSize(void* nativeWindowHandle) = 0;

        WindowResizeDelegate onWindowDrawableSizeChanged{};

    protected:

        static PlatformApplication* instance;

        Array<ApplicationMessageHandler*> messageHandlers{};
        Array<Delegate<void(void)>> tickHanders{};
    };

} // namespace CE
