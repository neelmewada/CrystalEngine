
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
			if (!IsValidObjectName(objectName) && !params.objectClass->IsSubclassOf<Package>())
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

			if (instance->HasAnyObjectFlags(OF_ClassDefaultInstance)) // Class Default Instance
			{
				instance->LoadDefaults();
			}
			else // Load default values from CDI
			{
				instance->LoadFromTemplate(const_cast<Object*>(instance->GetClass()->GetDefaultInstance()));
			}

			if (params.templateObject != nullptr)
			{
				instance->LoadFromTemplate(params.templateObject);
			}
            
			if (params.outer != nullptr)
				params.outer->AttachSubobject(instance);

			instance->OnAfterConstruct();
			return instance;
		}
		
	}

	/*
	*	Globals
	*/

	//extern Package* gTransientPackage;
    extern Package* gSettingsPackage;
	extern ResourceManager* gResourceManager;

	CORE_API Package* GetTransientPackage()
	{
		return ModuleManager::Get().GetLoadedModuleTransientPackage("Core");
	}

	static Package* LoadSettingsPackage()
	{
		return Package::LoadPackage(nullptr, PackagePath("/Game/Settings"), LOAD_Full);
	}

    CORE_API Package* GetSettingsPackage()
    {
		if (gSettingsPackage == nullptr)
			gSettingsPackage = LoadSettingsPackage();

		if (gSettingsPackage == nullptr)
			gSettingsPackage = CreateObject<Package>(nullptr, TEXT("/Game/Settings"));

        return gSettingsPackage;
    }

	CORE_API ResourceManager* GetResourceManager()
	{
		if (gResourceManager == nullptr)
			gResourceManager = CreateObject<ResourceManager>(nullptr, TEXT("ResourceManager"), OF_Transient);
		return gResourceManager;
	}

	CORE_API bool IsValidObjectName(const String& name)
	{
		if (name.IsEmpty())
			return false;

		for (int i = 0; i < name.GetLength(); i++)
		{
			char ch = name[i];
			if (!String::IsAlphabet(ch) && !String::IsNumeric(ch) && ch != '_' && ch != '-')
			{
				return false;
			}
		}
		return true;
	}

	CORE_API String FixObjectName(const String& name)
	{
		return name.Replace({ '\\', '/', ' ', '+', '=', ':', ';', '.', ',', '[', ']', '{', '}', '(', ')',
			'!', '@', '#', '$', '%', '^', '&', '*', '`', '~', '|' },
			'_');
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
