#pragma once

#include "Messaging/MessageBus.h"

namespace CE
{
    class ComponentApplication;

    class ComponentApplicationRequsts : public MBusTraits
    {
    public:

        // Application is a singleton - so only 1 handler allowed
        static const MBusHandlerPolicy HandlerPolicy = MBusHandlerPolicy::Single;

        /// Gets a pointer to the application singleton
        virtual ComponentApplication* GetApplication() = 0;

    };

    using ComponentApplicationBus = MessageBus<ComponentApplicationRequsts>;
    
} // namespace CE
