#pragma once
#include "Object.h"

namespace CE
{
	class EnumType;
	class ClassType;
	class StructType;
	class FieldType;
	class FunctionType;
	class Bundle;
	class ResourceManager;
	class Object;

	/* **********************************
	*	Global Functions
	*/

	//! @brief Returns true if the given object pointer is valid and not destroyed! 
	CORE_API bool IsValidObject(Object* object);

	/// Transient Bundle: Same lifetime as Core Module.
	///	Used to store temporary objects that are not saved to disk.
	CORE_API Bundle* GetGlobalTransient();

	/// @brief Returns the transient bundle of a specific module.
	CORE_API Bundle* GetTransient(const String& moduleName);

    CORE_API Bundle* GetSettingsBundle();

	/// Call this to unload all settings. Should be called in PreShutdown phase.
	CORE_API void UnloadSettings();

	CORE_API ResourceManager* GetResourceManager();

	namespace Internal
	{
		
		struct CORE_API ObjectCreateParams
		{
			ObjectCreateParams() = default;
			ObjectCreateParams(ClassType* objectClass)
				: objectClass(objectClass)
			{}

			Object* outer = nullptr;
			ClassType* objectClass = nullptr;
			String name{};
			Object* templateObject = nullptr;
			ObjectFlags objectFlags{};
			Uuid uuid = 0;
		};

		/// For internal use only
		CORE_API Object* CreateObjectInternal(const ObjectCreateParams& params);
		
	}

	CORE_API bool IsValidObjectName(const String& name);
	CORE_API String FixObjectName(const String& name);

	template<typename TClass> requires TIsBaseClassOf<Object, TClass>::Value
	TClass* CreateObject(Object* outer = (Object*)GetGlobalTransient(),
		String objectName = "",
		ObjectFlags flags = OF_NoFlags,
		ClassType* objectClass = TClass::Type(), 
		Object* templateObject = NULL,
		Uuid uuid = 0)
	{
		if (objectClass == nullptr || !objectClass->IsSubclassOf(TClass::Type()))
			return nullptr;

		Internal::ObjectCreateParams params{ objectClass };
		params.outer = outer;
		params.name = objectName;
		params.templateObject = templateObject;
		params.objectFlags = flags;
		params.uuid = uuid;
		
		return static_cast<TClass*>(Internal::CreateObjectInternal(params));
	}

	/* ***********************************
	*	Delegates
	*/

	template <class TRetType, class ... TArgs>
	Variant ScriptDelegate<TRetType(TArgs...)>::Invoke(const Array<Variant>& args) const
	{
		if (!isBound)
			return nullptr;

		if (IsFunction())
		{
			if (!IsValidObject(dstObject))
			{
				isBound = false;
				return nullptr;
			}
			return dstFunction->Invoke(dstObject, args);
		}

		if (IsLambda())
		{
			return lambda(args);
		}

		return nullptr;
	}

	struct CORE_API CoreObjectDelegates
	{
		CoreObjectDelegates() = delete;

		typedef MultiCastDelegate<void(ClassType*)> ClassRegistrationDelegate;

		static ClassRegistrationDelegate onClassRegistered;
		static ClassRegistrationDelegate onClassDeregistered;

		typedef MultiCastDelegate<void(StructType*)> StructRegistrationDelegate;

		static StructRegistrationDelegate onStructRegistered;
		static StructRegistrationDelegate onStructDeregistered;

		typedef MultiCastDelegate<void(EnumType*)> EnumRegistrationDelegate;

		static EnumRegistrationDelegate onEnumRegistered;
		static EnumRegistrationDelegate onEnumDeregistered;
	};

}
