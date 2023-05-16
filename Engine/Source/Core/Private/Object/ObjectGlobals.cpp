
#include "CoreMinimal.h"

namespace CE
{
	namespace Internal
	{
		CORE_API bool gIsDebugObject = false;

		CORE_API Object* StaticConstructObject(const ConstructObjectParams& params)
		{
			if (params.objectClass == nullptr || !params.objectClass->CanBeInstantiated() || !params.objectClass->IsObject())
				return nullptr;
			
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
			
			if (params.owner != nullptr && params.owner->IsPackage())
				instance->ownerPackage = (Package*)params.owner;
			if (params.owner != nullptr)
				params.owner->AttachSubobject(instance);
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

	/*
	*	Object Initializer
	*/

	ObjectInitializer::ObjectInitializer()
	{
		Initialize();
	}

	ObjectInitializer::ObjectInitializer(ObjectFlags flags) : objectFlags(flags)
	{
		Initialize();
	}

	void ObjectInitializer::Initialize()
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
