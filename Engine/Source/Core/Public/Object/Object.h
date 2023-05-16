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
    protected:
        Object();

        Object(const ObjectInitializer& initializer);
        
        virtual ~Object();

    private:

        void ConstructInternal();
        void ConstructInternal(ObjectInitializer* initializer);

    public:

        // - Getters & Setters -
        INLINE CE::String GetName() const
        {
            return name;
        }

        INLINE void SetName(CE::String newName)
        {
            this->name = newName;
        }
        
        INLINE UUID GetUuid() const
        {
            return uuid;
        }

        INLINE TypeId GetTypeId() const
        {
            return GetType()->GetTypeId();
        }
        
        INLINE ThreadId GetCreationThreadId() const
        {
            return creationThreadId;
        }

        INLINE ObjectFlags GetFlags() const
        {
            return objectFlags;
        }

        INLINE bool HasAllObjectFlags(ObjectFlags flags) const
        {
            return (objectFlags & flags) == flags;
        }

        INLINE bool HasAnyObjectFlags(ObjectFlags flags) const
        {
            return (objectFlags & flags) != 0;
        }

        INLINE void EnableObjectFlags(ObjectFlags flags)
        {
            objectFlags = objectFlags | flags;
        }

        INLINE void DisableObjectFlags(ObjectFlags flags)
        {
            objectFlags = objectFlags & ~flags;
        }

        INLINE bool IsDefaultInstance() const
        {
            return HasAnyObjectFlags(OF_ClassDefaultInstance);
        }

        INLINE bool IsTemplate() const
        {
            return HasAnyObjectFlags(OF_TemplateInstance);
        }

        INLINE bool IsTransient() const
        {
            return HasAnyObjectFlags(OF_Transient);
        }

        // Lifecycle

        void AttachSubobject(Object* subobject);
        
        void DetachSubobject(Object* subobject);

        void RequestDestroy();

        // - Public API -

        virtual bool IsAsset() { return false; }

        virtual bool IsPackage() { return false; }

        // - Config API -

        void LoadConfig(ClassType* configClass = NULL, String fileName = "");

    private:
        
        friend class Package;
        
        friend class EventBus;
        friend Object* Internal::StaticConstructObject(const Internal::ConstructObjectParams& params);

        template<typename T>
        friend struct Internal::TypeInfoImpl;
        
        /*
         *  Fields
         */

        CE::String name;
        CE::UUID uuid;

        ObjectFlags objectFlags = OF_NoFlags;

        // Subobject Lifecycle
		ObjectStore attachedObjects{};
        
        Object* outer = nullptr;
        Package* ownerPackage = nullptr;
        
        ThreadId creationThreadId{};
        Mutex mutex{};
    };
    
} // namespace CE

CE_RTTI_CLASS(CORE_API, CE, Object,
    CE_SUPER(),
    CE_NOT_ABSTRACT,
    CE_ATTRIBS(),
    CE_FIELD_LIST(
        CE_FIELD(name, Hidden)
        CE_FIELD(uuid, Hidden)
    ),
    CE_FUNCTION_LIST(
        
    )
)
