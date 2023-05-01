
#include "CoreMinimal.h"

namespace CE
{
	namespace Internal
	{

		CORE_API Object* StaticConstructObject(const ConstructObjectParams& params)
		{
			if (params.objectClass == nullptr || !params.objectClass->CanBeInstantiated() || !params.objectClass->IsObject())
				return nullptr;
			
			ObjectInitializer init = ObjectInitializer();
			init.objectFlags = params.objectFlags;
			init.name = params.name;
			init.uuid = 0;
            init.objectClass = params.objectClass;

			ObjectThreadContext::Get().PushInitializer(&init);

			auto instance = params.objectClass->CreateInstance();
			if (instance == nullptr)
            {
                ObjectThreadContext::Get().PopInitializer();
                return nullptr;
            }

			String name = params.name;
			if (instance->GetName().IsEmpty())
				name = params.objectClass->GenerateInstanceName(instance->GetUuid()).GetString();
			instance->SetName(name);

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
