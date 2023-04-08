#pragma once

#include "Messaging/MessageBus.h"

namespace CE
{
    class Engine;

    class ApplicationRequests
    {
    public:
        virtual void GetEngineRef(Engine** outEngineRef) {}
    };

    //using ApplicationBus = MessageBus<ApplicationRequests>;

    CE_EVENT_BUS(SYSTEM_API, ApplicationBus, ApplicationRequests);

} // namespace CE
