
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
			init.uuid = 0;
            init.objectClass = params.objectClass;

			if (params.isDebug)
			{
				gIsDebugObject = true;
				
				if (init.name != "Obj1" && init.name != "Obj2")
				{
					DEBUG_BREAK();
				}
			}

			ObjectThreadContext::Get().PushInitializer(&init);

			auto instance = params.objectClass->CreateInstance();
			if (instance == nullptr)
            {
                ObjectThreadContext::Get().PopInitializer();
                return nullptr;
            }

			if (params.isDebug)
			{
				gIsDebugObject = false;
				
				if (instance->GetName() != "Obj1" && instance->GetName() != "Obj2")
				{
					DEBUG_BREAK();
				}
			}

			// TODO: Add it later
			//if (params.owner != nullptr)
			//	params.owner->AttachSubobject(instance);
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
