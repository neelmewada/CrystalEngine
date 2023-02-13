#pragma once

#include "Messaging/MessageBus.h"

namespace CE
{
    class Engine;

    class ApplicationRequests : public MBusTraits
    {
    public:
        virtual void RunMainLoop() {}

        virtual void ExitMainLoop() {}

        virtual Engine* GetEngineRef() { return nullptr; }
    };

    //using ApplicationBus = MessageBus<ApplicationRequests>;

    CE_EVENT_BUS(SYSTEM_API, ApplicationBus);

} // namespace CE
