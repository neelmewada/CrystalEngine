#pragma once

#include "Containers/Array.h"
#include "Containers/String.h"

#include "Object/Object.h"

#include "Policies.h"

namespace CE
{

    
    class MBusTraits
    {
    public:

        static constexpr MBusAddressPolicy AddressPolicy = MBusAddressPolicy::Single;

        static constexpr MBusHandlerPolicy HandlerPolicy = MBusHandlerPolicy::Multiple;

    };

    class IBusInterface : public MBusTraits
    {
    public:

        virtual SIZE_T GetAddress() { return 0; }

    };

    /// Message bus class.
    /// Handler: A class inherited from IBusHandler that serves as an interface to the bus.
    /// BusTraits: Traits about the bus. Same as the Interface when unspecified.
    template<class Interface, class BusTraits = Interface>
    class MessageBus
    {
    public:

        using Handler = Interface;

        static void AddHandler(Handler* handler)
        {
            if (!Handlers.Exists(handler))
            {
                Handlers.Add(handler);
            }
        }

        static void RemoveHandler(Handler* handler)
        {
            Handlers.Remove(handler);
        }

        /// Dispatch message to all handlers if Single addressing mode. Does nothing if address mode is ById.
        template<typename... Args>
        static void DispatchMessage(auto function, Args... args)
        {
            if constexpr (BusTraits::AddressPolicy == MBusAddressPolicy::Single)
            {
                for (Interface* handler : Handlers)
                {
                    (handler->*function)(args...);
                }
            }
        }

        /// Dispatches message to handlers with the specified address in ById address mode. Address is ignored if Single addressing.
        template<typename... Args>
        static void DispatchMessage(auto function, SIZE_T address, Args... args)
        {
            if constexpr (BusTraits::AddressPolicy == MBusAddressPolicy::Single)
            {
                for (Interface* handler : Handlers)
                {
                    (handler->*function)(args...);
                }
            }
            else if constexpr (BusTraits::AddressPolicy == MBusAddressPolicy::ById)
            {
                for (Interface* handler : Handlers)
                {
                    if (handler->GetAddress() == address)
                    {
                        (handler->*function)(args...);
                    }
                }
            }
        }

    private:
        static Array<Interface*> Handlers;
    };

    template<class Interface, class BusTraits>
    Array<Interface*> MessageBus<Interface, BusTraits>::Handlers = {};
    
} // namespace CE

