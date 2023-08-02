#include "System.h"

namespace CE
{
	SYSTEM_API extern AssetDefinitionRegistry* gAssetDefinitionRegistry;

	AssetDefinition::~AssetDefinition()
	{

	}

	AssetDefinitionRegistry::AssetDefinitionRegistry()
	{
		if (!IsDefaultInstance()) // If not a class default instance
		{
			handle = CoreObjectDelegates::onClassRegistered.AddDelegateInstance(MemberDelegate(&Self::OnClassRegistered, this));
		}
	}

	AssetDefinitionRegistry::~AssetDefinitionRegistry()
	{
		if (handle != 0)
			CoreObjectDelegates::onClassRegistered.RemoveDelegateInstance(handle);
	}

	AssetDefinitionRegistry* AssetDefinitionRegistry::Get()
	{
		return gAssetDefinitionRegistry;
	}

	AssetDefinition* AssetDefinitionRegistry::FindAssetDefinitionForSourceAssetExtension(const String& extension)
	{
		for (auto assetDef : assetDefinitions)
		{
			if (assetDef->GetSourceExtensions().Exists(extension))
				return assetDef;
		}

		return nullptr;
	}

	void AssetDefinitionRegistry::OnClassRegistered(ClassType* classType)
	{
		if (classType != nullptr && classType->IsSubclassOf<AssetDefinition>() && classType->CanBeInstantiated())
		{
			AssetDefinition* instance = CreateObject<AssetDefinition>(this, "AssetDefinition", OF_NoFlags, classType);
			if (instance != nullptr)
			{
				assetDefinitions.Add(instance);
			}
		}
	}

} // namespace CE

