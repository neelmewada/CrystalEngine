#pragma once

#include "Messaging/MessageBus.h"

namespace CE
{

    enum ComponentTickBus
    {
        TICK_FIRST          = 0,

        TICK_PLACEMENT      = 50,

        TICK_INPUT          = 75,
        
        TICK_GAME           = 80,

        TICK_ANIMATION      = 100,

        TICK_PHYSICS_SYSTEM = 200,

        TICK_PHYSICS        = TICK_PHYSICS_SYSTEM + 1,

        TICK_ATTACHMENT     = 500,     ///< Suggested tick handler position for attachment components.

        TICK_PRE_RENDER     = 750,     ///< Suggested tick handler position to update render-related data.

        TICK_DEFAULT        = 1000,    ///< Default tick handler position when the handler is constructed.

        TICK_UI             = 2000,    ///< Suggested tick handler position for UI components.

        TICK_LAST           = 100000,  ///< Last position in the tick handler order.
    };
    
    class TickEvents : public IBusInterface
    {
    public:

        static constexpr MBusAddressPolicy AddressPolicy = MBusAddressPolicy::Single;

        static constexpr MBusHandlerPolicy HandlerPolicy = MBusHandlerPolicy::Multiple;

        virtual void OnTick(f32 deltaTime) = 0;

        virtual s32 GetTickOrder() { return 0; }
    };


    class TickRequests : public IBusInterface
    {
    public:

        virtual f32 GetTickDeltaTime() = 0;

    };

    using TickRequestBus = MessageBus<TickRequests>;

} // namespace CE
