#pragma once

#include "ApplicationMessageHandler.h"

namespace CE
{
    class PlatformWindow;
    
    class COREAPPLICATION_API PlatformApplication
    {
    public:

        static PlatformApplication* Create();

        PlatformApplication() = default;

        virtual ~PlatformApplication() = default;

        virtual void SetMessageHandler(ApplicationMessageHandler* handler)
        {
            this->messageHandler = handler;
        }

        virtual ApplicationMessageHandler* GetMessageHandler()
        {
            return messageHandler;
        }

        virtual void Initialize();
        virtual void PreShutdown();
        virtual void Shutdown();

        virtual void Tick();

        virtual PlatformWindow* InitMainWindow(const String& title, u32 width, u32 height, bool maximised, bool fullscreen) = 0;

        virtual PlatformWindow* GetMainWindow() = 0;

    protected:

        ApplicationMessageHandler* messageHandler = nullptr;
    };

} // namespace CE
