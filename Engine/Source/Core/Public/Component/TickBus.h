#pragma once

#include "Messaging/MessageBus.h"

namespace CE
{
    
    enum ComponentTickBus
    {
        TICK_FIRST = 0,       ///< First position in the tick handler order.

        TICK_PLACEMENT = 50,      ///< Suggested tick handler position for components that need to be early in the tick order.

        TICK_INPUT = 75,      ///< Suggested tick handler position for input components.

        TICK_GAME = 80,      ///< Suggested tick handler for game-related components.

        TICK_ANIMATION = 100,     ///< Suggested tick handler position for animation components.

        TICK_PHYSICS_SYSTEM = 200,     ///< Suggested tick handler position for physics systems. Note: This should only be used for the Physics System.

        TICK_PHYSICS = TICK_PHYSICS_SYSTEM + 1,  ///< Suggested tick handler position for physics components

        TICK_ATTACHMENT = 500,     ///< Suggested tick handler position for attachment components.

        TICK_PRE_RENDER = 750,     ///< Suggested tick handler position to update render-related data.

        TICK_DEFAULT = 1000,    ///< Default tick handler position when the handler is constructed.

        TICK_UI = 2000,    ///< Suggested tick handler position for UI components.

        TICK_LAST = 100000,  ///< Last position in the tick handler order.
    };

    class TickEvents : public IBusInterface
    {
    public:

        static constexpr MBusAddressPolicy AddressPolicy = MBusAddressPolicy::Single;

        static constexpr MBusHandlerPolicy HandlerPolicy = MBusHandlerPolicy::MultipleAndOrdered;

        virtual void OnTick(f32 deltaTime) = 0;

        virtual s32 GetTickOrder() { return 0; }

    protected:
        friend class IComponentApplication;

    };

    using TickBus = MessageBus<TickEvents>;


    class TickRequests : public MBusTraits
    {
    public:

        virtual f32 GetTickDeltaTime() = 0;

    };

    using TickRequestBus = MessageBus<TickRequests>;

} // namespace CE
