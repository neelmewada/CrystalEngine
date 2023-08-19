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
#include "SubClassType.h"

#include "ObjectThreadContext.h"
#include "ObjectGlobals.h"
#include "ObjectMap.h"

#if PAL_TRAIT_BUILD_TESTS
class Package_WriteRead_Test;
#endif

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
        virtual const Name& GetName() const
        {
            return name;
        }

        virtual void SetName(const Name& newName)
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

		bool IsTransient();

		inline u32 GetSubObjectCount() const { return attachedObjects.GetObjectCount(); }
		inline const ObjectMap& GetSubObjectMap() const { return attachedObjects; }

        // Lifecycle

        virtual void AttachSubobject(Object* subobject);
        
        virtual void DetachSubobject(Object* subobject);

		virtual bool HasSubobject(Object* subobject);

		bool ObjectPresentInHierarchy(Object* searchObject);

        void RequestDestroy();

        // - Public API -

        virtual bool IsAsset() { return false; }

        virtual bool IsPackage() { return false; }

		bool IsOfType(ClassType* classType);

		template<typename T>
		FORCE_INLINE bool IsOfType()
		{
			return IsOfType(T::Type());
		}

		Name GetPathInPackage();
        
		// Returns the package this object belongs to.
        Package* GetPackage();

		// Internal use only! Returns a list of all objects that this object and it's subobjects reference to.
		void FetchObjectReferences(HashMap<UUID, Object*>& outReferences);

        // - Config API -

        void LoadConfig(ClassType* configClass, String fileName);
        
    protected:

		// Lifecycle

		virtual void OnAfterConstruct() {}
		virtual void OnBeforeDestroy() {}

		virtual void OnSubobjectAttached(Object* subobject) {}

		virtual void OnSubobjectDetached(Object* subobject) {}

		virtual void OnAfterDeserialize() {}
		virtual void OnBeforeSerialize() {}

		// Protected API

		Object* CreateDefaultSubobject(ClassType* classType, const String& name, ObjectFlags flags = OF_NoFlags);

		template<typename TClass>
		TClass* CreateDefaultSubobject(const String& name, ObjectFlags flags = OF_NoFlags)
		{
			return (TClass*)CreateDefaultSubobject(TClass::Type(), name, flags);
		}

		void LoadFromTemplate(Object* templateObject);
        
		void LoadDefaults();

        void ConfigParseStruct(const String& value, void* instance, StructType* structType);
        
        void ConfigParsePOD(const String& value, void* instance, FieldType* field);
        
        virtual void OnAfterConfigLoad() {}

		void EmitSignal(const String& name, const Array<Variant>& args);

	private:

		static DelegateHandle BindInternal(void* sourceInstance, FunctionType* sourceFunction, Delegate<void(const Array<Variant>&)> delegate);

		template<typename ReturnType, typename... Args, std::size_t... Is>
		inline static DelegateHandle BindInternal(void* sourceInstance, FunctionType* sourceFunction, Delegate<ReturnType(Args...)> delegate, std::index_sequence<Is...>)
		{
			if (sourceInstance == nullptr || sourceFunction == nullptr || !delegate.IsValid())
				return false;

			if (sourceFunction->GetFunctionSignature() != GetFunctionSignature<Args...>())
			{
				CE_LOG(Error, All, "Cannot bind signal: Function signature mismatch");
				return false;
			}

			return BindInternal(sourceInstance, sourceFunction, Delegate<void(const Array<Variant>&)>([delegate](const Array<Variant>& args)
				{
					delegate.InvokeIfValid(((args.begin() + Is)->GetValue<Args>())...);
				}));
		}

	public:

		static void EmitSignal(void* signalInstance, const String& name, const Array<Variant>& args);

		static bool Bind(void* sourceInstance, FunctionType* sourceFunction, void* destinationInstance, FunctionType* destinationFunction);

		template<typename... Args>
		inline static DelegateHandle Bind(void* sourceInstance, FunctionType* sourceFunction, Delegate<void(Args...)> delegate)
		{
			return BindInternal<void, Args...>(sourceInstance, sourceFunction, delegate, std::make_index_sequence<sizeof...(Args)>());
		}

		template<typename TLambda>
		inline static DelegateHandle Bind(void* sourceInstance, FunctionType* sourceFunction, TLambda lambda)
		{
			typedef FunctionTraits<TLambda> Traits;
            typedef typename Traits::Tuple TupleType;
			typedef typename Traits::ReturnType ReturnType;
			typedef typename MakeDelegateType<TupleType, ReturnType>::DelegateType DelegateType;

			return Bind(sourceInstance, sourceFunction, DelegateType(lambda));
		}

		static void UnbindAllSignals(void* instance);
        
        static void Unbind(void* instance, DelegateHandle delegateInstance);

		static void UnbindSignals(void* toInstance, void* fromInstance);

    private:

		static void UnbindAllIncomingSignals(void* toInstance);
		static void UnbindAllOutgoingSignals(void* fromInstance);

#if PAL_TRAIT_BUILD_TESTS
		friend class ::Package_WriteRead_Test;
#endif
        
        friend class Package;
		friend class SavePackageContext;
        
        friend class EventBus;
        friend Object* Internal::StaticConstructObject(const Internal::ConstructObjectParams& params);

        template<typename T>
        friend struct Internal::TypeInfoImpl;
        
        /*
         *  Fields
         */

        Name name;
        UUID uuid;

        ObjectFlags objectFlags = OF_NoFlags;

        // Subobject Lifecycle
		ObjectMap attachedObjects{};
        
        Object* outer = nullptr;

		static HashMap<void*, Array<SignalBinding>> outgoingBindingsMap;
		static HashMap<void*, Array<SignalBinding>> incomingBindingsMap;
    };
    
} // namespace CE

CE_RTTI_CLASS(CORE_API, CE, Object,
    CE_SUPER(),
    CE_NOT_ABSTRACT,
    CE_ATTRIBS(),
    CE_FIELD_LIST(
        CE_FIELD(name, Hidden, ReadOnly) // name cannot be modified directly
        CE_FIELD(uuid, Hidden, ReadOnly) // uuid cannot be modified directly
		CE_FIELD(attachedObjects, Hidden, ReadOnly)
		CE_FIELD(outer, Hidden, ReadOnly)
    ),
    CE_FUNCTION_LIST(
        
    )
)

