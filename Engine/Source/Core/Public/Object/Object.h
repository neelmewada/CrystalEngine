#pragma once

#include "CoreTypes.h"
#include "Containers/String.h"

#include "ObjectMacros.h"
#include "RTTI/RTTIDefines.h"
#include "RTTI/RTTI.h"
#include "RTTI/Variant.h"
#include "Class.h"
#include "Enum.h"
#include "Field.h"
#include "Signal.h"

#include "ObjectThreadContext.h"
#include "ObjectGlobals.h"
#include "ObjectStore.h"

namespace CE
{

    class CORE_API Component;
    class CORE_API EventBus;

    class ClassType;
    class FieldType;
    class FunctionType;
    class Package;

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
        Object();

        Object(const ObjectInitializer& initializer);

        virtual ~Object();

    private:

        void ConstructInternal();
        void ConstructInternal(ObjectInitializer* initializer);

    public:

        // - Getters & Setters -
        CE_INLINE CE::String GetName() const
        {
            return name;
        }

        CE_INLINE void SetName(CE::String newName)
        {
            this->name = newName;
        }
        
        CE_INLINE UUID GetUuid() const
        {
            return uuid;
        }

        CE_INLINE TypeId GetTypeId() const
        {
            return GetType()->GetTypeId();
        }

        ObjectFlags GetFlags() const
        {
            return objectFlags;
        }

        bool HasAllFlags(ObjectFlags flags) const
        {
            return (objectFlags & flags) == flags;
        }

        bool HasAnyFlag(ObjectFlags flags) const
        {
            return (objectFlags & flags) != 0;
        }

        void EnableFlags(ObjectFlags flags)
        {
            objectFlags = (ObjectFlags)(objectFlags | flags);
        }

        void DisableFlags(ObjectFlags flags)
        {
            objectFlags = (ObjectFlags)(objectFlags & ~flags);
        }

        // Subobject API

        void AttachSubobject(Object* subobject);
        void DetachSubobject(Object* subobject);

		// Signal-Event API
        
        static bool Bind(Object* sourceObject, FunctionType* sourceSignal,
                         Object* destinationObject, FunctionType* destinationEvent);
        
        bool Bind(FunctionType* sourceSignal, Object* destinationObject, FunctionType* destinationEvent);
        
        bool Bind(Name sourceSignal, Object* destinationObject, Name destinationEvent);
        
        void Unbind(FunctionType* sourceSignal);
        void UnbindAll(Object* destinationObject);
        
        void FireSignal(Name signalName, const CE::Array<CE::Variant>& params);

        // - Public API -

        virtual bool IsAsset() { return false; }


        // - Config API -

        void LoadConfig(ClassType* configClass = NULL, String fileName = "");

    private:
        friend class EventBus;

        CE::String name;
        CE::UUID uuid;

        ObjectFlags objectFlags = OF_NoFlags;

        // Event System
        Array<EventBus*> subscribedBuses{};
        
        // Signal System
        HashMap<Name, Array<SignalBinding>> signalNameToBindingsMap{};
        Array<Object*> incomingSignalBinders{};

        // Object Lifecycle
		ObjectStore attachedObjects{};
        
        Object* outer = nullptr;
    };
    
} // namespace CE

CE_RTTI_CLASS(CORE_API, CE, Object,
    CE_SUPER(),
    CE_NOT_ABSTRACT,
    CE_ATTRIBS(),
    CE_FIELD_LIST(
        CE_FIELD(name, Hidden)
        CE_FIELD(uuid, Hidden)
        CE_FIELD(attachedObjects, Hidden)
    ),
    CE_FUNCTION_LIST(
        
    )
)

