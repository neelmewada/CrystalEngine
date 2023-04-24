#pragma once

#include "Messaging/MessageBus.h"

namespace CE
{
    class Engine;

    class ApplicationRequests
    {
    public:
        
    };

    CE_EVENT_BUS(SYSTEM_API, ApplicationBus, ApplicationRequests);

} // namespace CE
