#pragma once

#include "CoreTypes.h"
#include "Containers/String.h"

#include "ObjectMacros.h"
#include "RTTI.h"
#include "Variant.h"
#include "Class.h"
#include "Enum.h"
#include "Field.h"
#include "Signal.h"

#include "ConfigParser.h"

namespace CE
{

    class CORE_API Component;
    class CORE_API EventBus;

    template<typename TObject>
    class IObjectUpdateListener
    {
    public:
        virtual void OnObjectUpdated(TObject* component) {}
    };

    template<typename T>
    class IValueUpdateListener
    {
    public:
        virtual void OnValueUpdated(const T& newValue) {}
    };

    class CORE_API Object
    {
        CE_CLASS(Object)
    public:
        Object(UUID uuid = UUID());
        Object(CE::Name name, UUID uuid = UUID());

        virtual ~Object();

        // - Getters & Setters -
        CE_INLINE CE::Name GetName() const
        {
            return name;
        }

        CE_INLINE void SetName(CE::Name newName)
        {
            this->name = newName;
        }
        
        CE_INLINE UUID GetUuid() const
        {
            return uuid;
        }
        
    public: // Signals
        
        static bool Bind(Object* sourceObject, FunctionType* sourceSignal,
                         Object* destinationObject, FunctionType* destinationEvent);
        
        bool Bind(FunctionType* sourceSignal, Object* destinationObject, FunctionType* destinationEvent);
        
        bool Bind(Name sourceSignal, Object* destinationObject, Name destinationEvent);
        
        void Unbind(FunctionType* sourceSignal);
        void UnbindAll(Object* destinationObject);
        
        void FireSignal(Name signalName, const CE::Array<CE::Variant>& params);
        
    private:
        friend class EventBus;

        CE::Name name;
        CE::UUID uuid;

        // Event System
        Array<EventBus*> subscribedBuses{};
        
        // Signal System
        HashMap<Name, SignalBinding> signalBindings{};
        Array<Object*> incomingSignalBinders{};
        
    protected:
        //CE::Array<Component*> components{};
    };
    
} // namespace CE

CE_RTTI_CLASS(CORE_API, CE, Object,
    CE_SUPER(),
    CE_ABSTRACT,
    CE_ATTRIBS(),
    CE_FIELD_LIST(
        CE_FIELD(name, Hidden)
        CE_FIELD(uuid, Hidden)
    ),
    CE_FUNCTION_LIST(
        
    )
)
