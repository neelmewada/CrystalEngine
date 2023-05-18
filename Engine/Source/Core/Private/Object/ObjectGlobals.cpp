
#include "CoreMinimal.h"

namespace CE
{
	namespace Internal
	{

		CORE_API Object* StaticConstructObject(const ConstructObjectParams& params)
		{
			if (params.objectClass == nullptr || !params.objectClass->CanBeInstantiated() || !params.objectClass->IsObject())
				return nullptr;

			const auto& objectName = params.name;
			if (!IsValidObjectName(objectName))
			{
				CE_LOG(Error, All, "Failed to create object. Invalid name passed: {}", objectName);
				return nullptr;
			}
			
			ObjectInitializer init = ObjectInitializer();
			init.objectFlags = params.objectFlags;
			init.name = params.name;
			init.uuid = params.uuid;
            init.objectClass = params.objectClass;

			ObjectThreadContext::Get().PushInitializer(&init);

			auto instance = params.objectClass->CreateInstance(); // The constructor automatically pops ObjectInitializer from stack
			if (instance == nullptr)
            {
                ObjectThreadContext::Get().PopInitializer();
                return nullptr;
            }
            
			if (params.outer != nullptr)
				params.outer->AttachSubobject(instance);
			return instance;
		}
		
	}

	/*
	*	Global Function
	*/

	extern Package* gTransientPackage;

	CORE_API Package* GetTransientPackage()
	{
		return gTransientPackage;
	}

	CORE_API bool IsValidObjectName(const String& name)
	{
		if (name.IsEmpty())
			return false;

		for (int i = 0; i < name.GetLength(); i++)
		{
			char ch = name[i];
			if (ch == '.' || ch == ' ' || ch == '-')
			{
				return false;
			}
		}
		return true;
	}

	/*
	*	Object Initializer
	*/

	ObjectInitializer::ObjectInitializer()
	{
		
	}

	ObjectInitializer::ObjectInitializer(ObjectFlags flags) : objectFlags(flags)
	{
		
	}

	/* ***************************************
	*	Delegates
	*/

	CoreObjectDelegates::ClassRegistrationDelegate CoreObjectDelegates::onClassRegistered{};
	CoreObjectDelegates::ClassRegistrationDelegate CoreObjectDelegates::onClassDeregistered{};

	CoreObjectDelegates::StructRegistrationDelegate CoreObjectDelegates::onStructRegistered{};
	CoreObjectDelegates::StructRegistrationDelegate CoreObjectDelegates::onStructDeregistered{};

} // namespace CE
