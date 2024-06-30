#pragma once

#include "CoreTypes.h"
#include "Containers/String.h"

#include "ObjectMacros.h"
#include "RTTI/RTTIDefines.h"
#include "RTTI/RTTI.h"
#include "RTTI/Variant.h"
#include "Function.h"
#include "Class.h"
#include "Enum.h"
#include "Signal.h"
#include "SubClassType.h"

#include "ObjectCreationContext.h"
#include "ObjectGlobals.h"
#include "ObjectMap.h"

#include "Event.h"

#if PAL_TRAIT_BUILD_TESTS
class Bundle_WriteRead_Test;
#endif

namespace CE
{

    class ClassType;
    class FieldType;
    class FunctionType;
    class Bundle;

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
        
        virtual ~Object();

    private:

        void ConstructInternal();
        void ConstructInternal(Internal::ObjectCreateParams* initializer);

    public:

        // - Getters & Setters -
        const Name& GetName() const
        {
            return name;
        }

        virtual void SetName(const Name& newName);
        
        INLINE Uuid GetUuid() const
        {
            return uuid;
        }

        INLINE TypeId GetTypeId() const
        {
            return GetType()->GetTypeId();
        }

        INLINE ObjectFlags GetFlags() const
        {
            return objectFlags;
        }

		INLINE Object* GetOuter() const
		{
			return outer;
		}

		bool ParentExistsInChain(Object* parent) const;

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

		bool IsTransient() const;

		inline u32 GetSubObjectCount() const { return attachedObjects.GetObjectCount(); }
        inline Object* GetSubobject(int index) const { return attachedObjects.GetObjectAt(index); }
		inline const ObjectMap& GetSubObjectMap() const { return attachedObjects; }

        // Lifecycle

        virtual void AttachSubobject(Object* subobject);
        
        virtual void DetachSubobject(Object* subobject);

		virtual bool HasSubobject(Object* subobject);

		bool IsObjectPresentInHierarchy(Object* searchObject);

        void RequestDestroy();

		FORCE_INLINE void Destroy()
		{
			RequestDestroy();
		}

        // - Public API -

        virtual bool IsAsset() const { return false; }

        virtual bool IsBundle() const { return false; }

		bool IsOfType(ClassType* classType) const;

		template<typename T>
		FORCE_INLINE bool IsOfType() const
		{
			return IsOfType(T::StaticType());
		}

		template<typename TClass> requires TIsBaseClassOf<CE::Object, TClass>::Value
		FORCE_INLINE static TClass* CastTo(Object* instance)
		{
			if (instance == nullptr || !instance->IsOfType<TClass>())
			{
				return nullptr;
			}
			return (TClass*)instance;
		}

        virtual u64 ComputeMemoryFootprint();

		Name GetPathInBundle();
        
		// Returns the bundle this object belongs to.
        Bundle* GetBundle();

		// Internal use only! Returns a list of all objects that this object and it's subobjects reference to.
		void FetchObjectReferences(HashMap<Uuid, Object*>& outReferences);

		Object* Clone(String cloneName = "", bool deepClone = true);

		void LoadFromTemplate(Object* templateObject);

        // - Config API -

        void LoadConfig(ClassType* configClass, String fileName);

        // - Updates -

        //! @brief Called when a field inside this object is edited by an editor
        //! @param field The field that was edited.
        virtual void OnFieldEdited(FieldType* field);

        virtual void OnFieldValidate() {}

    protected:

		void LoadFromTemplateHelper(HashMap<Uuid, Object*>& originalToClonedObjectMap, Object* templateObject);

		void LoadFromTemplateFieldHelper(HashMap<Uuid, Object*>& originalToClonedObjectMap,
			Field* srcField, void* srcInstance, Field* dstField, void* dstInstance);

		Object* CloneHelper(HashMap<Uuid, Object*>& originalToClonedObjectMap, Object* outer, String cloneName, bool deepClone);

		// Lifecycle

		virtual void OnAfterConstruct() {}
		virtual void OnBeforeDestroy() {}

		virtual void OnSubobjectAttached(Object* subobject) {}

		virtual void OnSubobjectDetached(Object* subobject) {}

		virtual void OnAfterDeserialize() {}
		virtual void OnBeforeSerialize() {}

		// Protected API

		Object* CreateDefaultSubobject(ClassType* classType, const String& name, ObjectFlags flags = OF_NoFlags);

		template<typename TClass> requires TIsBaseClassOf<Object, TClass>::Value
		TClass* CreateDefaultSubobject(const String& name, ObjectFlags flags = OF_NoFlags)
		{
			return (TClass*)CreateDefaultSubobject(TClass::Type(), name, flags | OF_DefaultSubobject);
		}

		template<typename TClass> requires TIsBaseClassOf<Object, TClass>::Value
		TClass* CreateDefaultSubobject(ClassType* classType, const String& name, ObjectFlags flags = OF_NoFlags)
		{
			if (!classType->IsSubclassOf<TClass>())
				return nullptr;
			return (TClass*)CreateDefaultSubobject(classType, name, flags | OF_DefaultSubobject);
		}
		
		void LoadDefaults();

        void ConfigParseStruct(const String& value, void* instance, StructType* structType);
        
        void ConfigParseField(const String& value, void* instance, FieldType* field);
        
        virtual void OnAfterConfigLoad() {}


	private:

        void UnbindAllEvents();

        void OnAfterConstructInternal();

		Object* GetDefaultSubobject(ClassType* classType, const String& name);

		void FetchObjectReferencesInStructField(HashMap<Uuid, Object*>& outReferences, StructType* structType, void* structInstance);

    private:

#if PAL_TRAIT_BUILD_TESTS
		friend class ::Bundle_WriteRead_Test;
#endif
        
        friend class Bundle;
		friend class SaveBundleContext;
        friend class BinaryDeserializer;
        friend class FieldType;
        
        friend class EventBus;
        friend Object* Internal::CreateObjectInternal(const Internal::ObjectCreateParams& params);

        template<typename T>
        friend struct Internal::TypeInfoImpl;
        
        /*
         *  Fields
         */

        Name name;
        Uuid uuid;

        // Subobject Lifecycle
		ObjectMap attachedObjects{};
        
        Object* outer = nullptr;

        ObjectFlags objectFlags = OF_NoFlags;


    };
    
} // namespace CE

CE_RTTI_CLASS(CORE_API, CE, Object,
    CE_SUPER(),
    CE_NOT_ABSTRACT,
    CE_ATTRIBS(),
    CE_FIELD_LIST(
        CE_FIELD(name, Hidden, ReadOnly, Internal) // name cannot be modified directly
        CE_FIELD(uuid, Hidden, ReadOnly, Internal) // uuid cannot be modified directly
		CE_FIELD(attachedObjects, Hidden, ReadOnly)
		CE_FIELD(outer, Hidden, ReadOnly)
    ),
    CE_FUNCTION_LIST(
        
    )
)
