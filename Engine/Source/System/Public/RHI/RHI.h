#pragma once

#include "Messaging/MessageBus.h"

#include "RHIResources.h"

namespace CE
{

    class SYSTEM_API DynamicRHI : public CE::IBusInterface
    {
    public:
        
        static constexpr MBusAddressPolicy AddressPolicy = MBusAddressPolicy::Single;
        
        static constexpr MBusHandlerPolicy HandlerPolicy = MBusHandlerPolicy::Single;
        
        virtual ~DynamicRHI() = default;
        
        // Lifecycle
        virtual void Initialize() = 0;
        virtual void PreShutdown() = 0;
        virtual void Shutdown() = 0;

        virtual RHIGraphicsBackend GetGraphicsBackend() = 0;
    };

    using RHIBus = MessageBus<DynamicRHI>;
    
} // namespace CE
