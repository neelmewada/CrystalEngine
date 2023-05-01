#pragma once

namespace CE
{
	class EnumType;
	class ClassType;
	class StructType;
	class FieldType;
	class FunctionType;
	class Package;

	/* **********************************
	*	Global Functions
	*/

	/// Transient Package: Same lifetime as Core Module.
	///	Used to store temporary objects that are not saved to disk.
	CORE_API Package* GetTransientPackage();

	namespace Internal
	{

		struct CORE_API ConstructObjectParams
		{
			ConstructObjectParams() = default;
			ConstructObjectParams(ClassType* objectClass)
				: objectClass(objectClass)
			{}

			Object* owner = nullptr;
			ClassType* objectClass = nullptr;
			String name{};
			Object* templateObject = nullptr;
			ObjectFlags objectFlags{};
		};

		/// For internal use only
		CORE_API Object* StaticConstructObject(const ConstructObjectParams& params);
		
	}

	template<typename TClass> requires std::is_base_of<Object, TClass>::value
	TClass* NewObject(Object* owner = (Object*)GetTransientPackage(), 
		String objectName = "", 
		ObjectFlags flags = OF_NoFlags,
		ClassType* objectClass = TClass::Type(), 
		Object* templateObject = NULL)
	{
		if (objectClass == nullptr || !objectClass->IsSubclassOf(TClass::Type()))
			return nullptr;

		Internal::ConstructObjectParams params{ objectClass };
		params.owner = owner;
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

		friend Object* Internal::StaticConstructObject(const Internal::ConstructObjectParams& params);

		/// Default constructor.
		ObjectInitializer();

		ObjectInitializer(ObjectFlags flags);


		ObjectFlags GetObjectFlags() const
		{
			return objectFlags;
		}

	private:
		void Initialize();

		ObjectFlags objectFlags{};
		String name{};
		UUID uuid{};
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