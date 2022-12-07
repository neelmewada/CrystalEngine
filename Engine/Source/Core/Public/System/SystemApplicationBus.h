#pragma once

#include "Messaging/MessageBus.h"

namespace CE
{

    class SystemApplication;

    /// Interface that components can use to make requests to the main application.
    class CORE_API SystemApplicationRequests : public IBusInterface
    {
    public:

        /// Gets a pointer to the application
        virtual SystemApplication* GetApplication() = 0;

    };

    class SystemApplicationRequestsBusTraits : public MBusTraits
    {
    public:

        static constexpr MBusAddressPolicy AddressPolicy = MBusAddressPolicy::Single;

        static constexpr MBusHandlerPolicy HandlerPolicy = MBusHandlerPolicy::Single;

    };

    using SystemApplicationBus = MessageBus<SystemApplicationRequests, SystemApplicationRequestsBusTraits>;
    
} // namespace CE
