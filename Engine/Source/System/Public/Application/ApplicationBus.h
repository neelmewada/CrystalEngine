#pragma once

#include "Messaging/MessageBus.h"

namespace CE
{
    class Engine;

    class ApplicationRequests : public MBusTraits
    {
    public:

        static constexpr MBusAddressPolicy AddressPolicy = MBusAddressPolicy::Single;

        static constexpr MBusHandlerPolicy HandlerPolicy = MBusHandlerPolicy::Single;

        virtual void RunMainLoop() {}

        virtual void ExitMainLoop() {}

        virtual Engine* GetEngineRef() { return nullptr; }
    };

    using ApplicationBus = MessageBus<ApplicationRequests>;
    
} // namespace CE
