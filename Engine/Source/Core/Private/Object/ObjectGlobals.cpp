
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
			init.objectFlags = params.objectFlags | OF_InsideConstructor;
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

			instance->objectFlags &= ~OF_InsideConstructor;

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
			{
				params.outer->AttachSubobject(instance);
			}

			//if (!instance->HasAnyObjectFlags(OF_ClassDefaultInstance))
				instance->OnAfterConstructInternal();
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
		return ModuleManager::Get().GetLoadedModuleTransientPackage(MODULE_NAME);
	}

	CORE_API Package* GetTransientPackage(const String& moduleName)
	{
		return ModuleManager::Get().GetLoadedModuleTransientPackage(moduleName);
	}

	static Package* LoadSettingsPackage()
	{
		return Package::LoadPackage(nullptr, Name("/Game/Settings"), LOAD_Full);
	}

	CORE_API Package* GetSettingsPackage()
    {
		if (gSettingsPackage == nullptr)
			gSettingsPackage = LoadSettingsPackage();

		if (gSettingsPackage == nullptr)
			gSettingsPackage = CreateObject<Package>(nullptr, TEXT("/Game/Settings"));

        return gSettingsPackage;
    }

	CORE_API void UnloadSettings()
	{
		if (gSettingsPackage != nullptr)
		{
			gSettingsPackage->Destroy();
			gSettingsPackage = nullptr;
		}
	}

	CORE_API ResourceManager* GetResourceManager()
	{
		if (gResourceManager == nullptr)
			gResourceManager = CreateObject<ResourceManager>(GetTransientPackage(MODULE_NAME), TEXT("ResourceManager"), OF_Transient);
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

	CoreObjectDelegates::EnumRegistrationDelegate CoreObjectDelegates::onEnumRegistered{};
	CoreObjectDelegates::EnumRegistrationDelegate CoreObjectDelegates::onEnumDeregistered{};

} // namespace CE
