#pragma once

#include "Containers/Array.h"
#include "Containers/String.h"

#include "Object/Object.h"

#include "Policies.h"

#define MBUS_EVENT(_MBUS, ...) _MBUS::Broadcast(&_MBUS::Events::__VA_ARGS__)

namespace CE
{
    typedef SIZE_T BusAddress;

    class MBusTraits
    {
    public:

        static constexpr MBusAddressPolicy AddressPolicy = MBusAddressPolicy::Single;

        static constexpr MBusHandlerPolicy HandlerPolicy = MBusHandlerPolicy::Multiple;

    };

    class IBusInterface : public MBusTraits
    {
    public:

        virtual BusAddress GetAddress() { return 0; }

        virtual SIZE_T GetHandlerOrder() { return 0; }

    };

    /// Message bus class.
    /// Handler: A class inherited from IBusHandler that serves as an interface to the bus.
    /// BusTraits: Traits about the bus. Same as the Interface when unspecified.
    template<class Interface, class BusTraits = Interface>
    class MessageBus
    {
    public:

        using Handler = Interface;
        using Events = Interface;

        static void BusConnect(Handler* handler)
        {
            if (!Handlers.Exists(handler))
            {
                Handlers.Add(handler);
            }
        }

        static void BusDisconnect(Handler* handler)
        {
            Handlers.Remove(handler);
        }

        /// Dispatch message to all handlers if Single addressing mode. Does nothing if address mode is ById.
        template<typename... Args>
        static void Broadcast(auto function, Args... args)
        {
            if constexpr (BusTraits::AddressPolicy == MBusAddressPolicy::Single)
            {
                for (Interface* handler : Handlers)
                {
                    if (handler == nullptr)
                        continue;

                    (handler->*function)(args...);

                    if constexpr (BusTraits::HandlerPolicy == MBusHandlerPolicy::Single)
                    {
                        break;
                    }
                }
            }
        }

        /// Dispatches message to handlers with the specified address in ById address mode. Address is ignored if Single addressing.
        template<typename... Args>
        static void Broadcast(auto function, BusAddress address, Args... args)
        {
            if constexpr (BusTraits::AddressPolicy == MBusAddressPolicy::Single)
            {
                for (Interface* handler : Handlers)
                {
                    if (handler == nullptr)
                        continue;

                    (handler->*function)(args...);

                    if constexpr (BusTraits::HandlerPolicy == MBusHandlerPolicy::Single)
                    {
                        break;
                    }
                }
            }
            else if constexpr (BusTraits::AddressPolicy == MBusAddressPolicy::ById)
            {
                for (Interface* handler : Handlers)
                {
                    if (handler == nullptr)
                        continue;

                    if (handler->GetAddress() == address)
                    {
                        (handler->*function)(args...);

                        if constexpr (BusTraits::HandlerPolicy == MBusHandlerPolicy::Single)
                        {
                            break;
                        }
                    }
                }
            }
        }

    private:
        static Array<Interface*> Handlers;
    };

    template<class Interface, class BusTraits>
    Array<Interface*> MessageBus<Interface, BusTraits>::Handlers = {};

    class CORE_API EventBus
    {
    public:

        struct Subscriber
        {
            FunctionType* function;
            Object* object;
        };

        template<typename... Args>
        void Publish(Name eventName, Args... args)
        {
            for (const auto& subscriber : subscribers)
            {
                if (subscriber.function == nullptr)
                    continue;
                try
                {
                    subscriber.function->Invoke(subscriber.object, { args... });
                }
                catch (CE::VariantCastException exc)
                {
                    continue;
                }
            }
        }

        void AddSubscriber(Object* object)
        {
            if (object == nullptr)
                return;

            subscribers.Add(object);
        }

        void RemoveSubscriber(Object* object)
        {
            for (int i = 0; i < subscribers.GetSize(); i++)
            {
                if (subscribers[i] == object)
                {
                    object->subscribedBuses.Remove(this);
                    subscribers.RemoveAt(i);
                    break;
                }
            }
        }

        virtual Name GetBusName() = 0;

    private:
        friend class Object;

        Array<Object*> subscribers{};
    };
    
} // namespace CE

#define CE_CONNECT(BusName, Subscriber) BusName::Get().AddSubscriber(Subscriber);

#define CE_DISCONNECT(BusName, Subscriber) BusName::Get().RemoveSubscriber(Subscriber);

#define CE_EVENT_BUS(API, BusName)\
class API BusName : public CE::EventBus\
{\
private:\
    BusName() {}\
public:\
    static BusName& Get()\
    {\
        static BusName instance{};\
        return instance;\
    }\
    virtual Name GetBusName() override { return #BusName; }\
};

