#pragma once

namespace CE
{
	class EnumType;
	class ClassType;
	class StructType;
	class FieldType;
	class FunctionType;
	class Package;
	class ResourceManager;

	/* **********************************
	*	Global Functions
	*/

	/// Transient Package: Same lifetime as Core Module.
	///	Used to store temporary objects that are not saved to disk.
	CORE_API Package* GetTransientPackage();

    CORE_API Package* GetSettingsPackage();

	/// Call this to unload all settings. Should be called in PreShutdown phase.
	CORE_API void UnloadSettings();

	CORE_API ResourceManager* GetResourceManager();

	namespace Internal
	{
		
		struct CORE_API ConstructObjectParams
		{
			ConstructObjectParams() = default;
			ConstructObjectParams(ClassType* objectClass)
				: objectClass(objectClass)
			{}

			Object* outer = nullptr;
			ClassType* objectClass = nullptr;
			String name{};
			Object* templateObject = nullptr;
			ObjectFlags objectFlags{};
			UUID uuid = 0;
		};

		/// For internal use only
		CORE_API Object* StaticConstructObject(const ConstructObjectParams& params);
		
	}

	CORE_API bool IsValidObjectName(const String& name);
	CORE_API String FixObjectName(const String& name);

	template<typename TClass> requires TIsBaseClassOf<Object, TClass>::Value
	TClass* CreateObject(Object* outer = (Object*)GetTransientPackage(),
		String objectName = "",
		ObjectFlags flags = OF_NoFlags,
		ClassType* objectClass = TClass::Type(), 
		Object* templateObject = NULL)
	{
		if (objectClass == nullptr || !objectClass->IsSubclassOf(TClass::Type()))
			return nullptr;

		Internal::ConstructObjectParams params{ objectClass };
		params.outer = outer;
		params.name = objectName;
		params.templateObject = templateObject;
		params.objectFlags = flags;
		
		return static_cast<TClass*>(Internal::StaticConstructObject(params));
	}

	/* *************************************
	*	Object Initializer
	*/

	class CORE_API ObjectInitializer
	{
	public:
		friend class Object;

		friend Object* Internal::StaticConstructObject(const Internal::ConstructObjectParams&);

		/// Default constructor.
		ObjectInitializer();

		ObjectInitializer(ObjectFlags flags);


		ObjectFlags GetObjectFlags() const
		{
			return objectFlags;
		}

		String GetName() const
		{
			return name;
		}

		UUID GetUuid() const
		{
			return uuid;
		}

		ClassType* GetObjectClass() const
		{
			return objectClass;
		}

	private:
		

        ClassType* objectClass = nullptr;
		ObjectFlags objectFlags{};
		String name{};
		UUID uuid{};
        Package* package = nullptr;
	};

	/* ***********************************
	*	Delegates
	*/

	struct CORE_API CoreObjectDelegates
	{
		CoreObjectDelegates() = delete;

		typedef MultiCastDelegate<void(ClassType*)> ClassRegistrationDelegate;

		static ClassRegistrationDelegate onClassRegistered;
		static ClassRegistrationDelegate onClassDeregistered;

		typedef MultiCastDelegate<void(StructType*)> StructRegistrationDelegate;

		static StructRegistrationDelegate onStructRegistered;
		static StructRegistrationDelegate onStructDeregistered;

#if PAL_TRAIT_WITH_EDITOR_DATA

#endif
	};

}
