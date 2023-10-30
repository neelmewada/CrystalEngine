#pragma once

#include "Containers/Array.h"
#include "Containers/String.h"

#include "Logger/Logger.h"

#include "Object/Object.h"

#include "Policies.h"
#include "Event.h"

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

        template<typename... Args>
        CE::Variant Publish(Name eventName, Args... args)
        {
            CE::Variant result{};

            for (auto subscriber : subscribers)
            {
                if (subscriber == nullptr)
                    continue;

                auto type = subscriber->GetType();

                if (!type->IsClass())
                    continue;

                auto classType = (ClassType*)type;

                auto functions = classType->FindAllFunctionsWithName(eventName);

                CE::Array<SIZE_T> typeIds = { ((SIZE_T)TYPEID(Args))... };
                SIZE_T signature = GetCombinedHashes(typeIds);

                bool funcFound = false;
                
                for (auto function : functions)
                {
                    if (function->GetFunctionSignature() == signature)
                    {
                        try
                        {
                            result = function->Invoke(subscriber, { args... });
                            funcFound = true;
                        }
                        catch (...)
                        {
                            continue;
                        }
                        break;
                    }
                }

                if (!funcFound)
                {
                    CE_LOG(Error, All, "Failed to publish {} event for subscriber {}\nCould not find a function with name {} and signature {}",
                        eventName, subscriber->GetName(), eventName, signature);
                }
            }

            return result;
        }

        template<typename... Args>
        CE::Variant Publish(Uuid messageTarget, Name eventName, Args... args)
        {
            CE::Variant result{};

            for (auto subscriber : subscribers)
            {
                if (subscriber == nullptr || subscriber->GetUuid() != messageTarget)
                    continue;

                auto type = subscriber->GetType();

                if (!type->IsClass())
                    continue;

                auto classType = (ClassType*)type;

                auto functions = classType->FindAllFunctionsWithName(eventName);

                CE::Array<SIZE_T> typeIds = { ((SIZE_T)TYPEID(Args))... };
                SIZE_T signature = GetCombinedHashes(typeIds);

                bool funcFound = false;

                for (auto function : functions)
                {
                    if (function->GetFunctionSignature() == signature)
                    {
                        try
                        {
                            result = function->Invoke(subscriber, { args... });
                            if (result.HasValue() && result.GetValueTypeId() == TYPEID(EventResult))
                            {
                                auto value = result.template GetValue<EventResult>();
                                if (value == EventResult::HandleAndStopPropagation)
                                {
                                    break;
                                }
                            }
                            funcFound = true;
                        }
                        catch (...)
                        {
                            continue;
                        }
                        break;
                    }
                }

                if (!funcFound)
                {
                    CE_LOG(Error, All, "Failed to publish {} event for subscriber {}\nCould not find a function with name {} and signature {}",
                        eventName, subscriber->GetName(), eventName, signature);
                }
            }

            return result;
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
                    subscribers.RemoveAt(i);
                    break;
                }
            }
        }

        virtual Name GetBusName() = 0;

    protected:
        friend class Object;

        Array<Object*> subscribers{};
    };
    
} // namespace CE

#define CE_CONNECT(BusName, Subscriber) BusName::Get().AddSubscriber(Subscriber);

#define CE_DISCONNECT(BusName, Subscriber) BusName::Get().RemoveSubscriber(Subscriber);

#define CE_EVENT_BUS(API, BusName, HandlerInterface)\
class API BusName : public CE::EventBus\
{\
private:\
    BusName() {}\
public:\
    using Interface = HandlerInterface;\
    static BusName& Get()\
    {\
        static BusName instance{};\
        return instance;\
    }\
    virtual CE::Name GetBusName() override { return #BusName; }\
};

#define CE_PUBLISH(BusName, EventName, ...) BusName::Get().Publish(#EventName, ##__VA_ARGS__)
#define CE_PUBLISH_TO(MessageTarget, BusName, EventName, ...) BusName::Get().Publish(MessageTarget, #EventName, ##__VA_ARGS__)

