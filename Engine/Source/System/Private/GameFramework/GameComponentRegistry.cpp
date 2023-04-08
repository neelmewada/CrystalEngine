
#include "CoreMinimal.h"

#include "GameFramework/GameComponentRegistry.h"

namespace CE
{

	void GameComponentRegistry::Register(TypeId componentTypeId)
	{
		auto typeInfo = GetTypeInfo(componentTypeId);
		
		if (typeInfo == nullptr || !typeInfo->IsObject() || !typeInfo->IsAssignableTo(TYPEID(GameComponent)))
			return;
		if (registryMap1.KeyExists(componentTypeId))
			return;
		
		registryMap1.Add({ componentTypeId, (ClassType*)typeInfo });
		registryMap2.Add({ typeInfo->GetName(), (ClassType*)typeInfo});
		registryArray.Add((ClassType*)typeInfo);
	}

	void GameComponentRegistry::Register(Name componentName)
	{
		auto typeInfo = GetTypeInfo(componentName);

		if (typeInfo == nullptr || !typeInfo->IsObject() || !typeInfo->IsAssignableTo(TYPEID(GameComponent)))
			return;
		if (registryMap2.KeyExists(componentName))
			return;

		registryMap1.Add({ typeInfo->GetTypeId(), (ClassType*)typeInfo});
		registryMap2.Add({ componentName, (ClassType*)typeInfo });
		registryArray.Add((ClassType*)typeInfo);
	}

	ClassType* GameComponentRegistry::FindEntry(Name componentName)
	{
		if (!registryMap2.KeyExists(componentName))
			return nullptr;

		return registryMap2[componentName];
	}

	ClassType* GameComponentRegistry::FindEntry(TypeId componentTypeId)
	{
		if (!registryMap1.KeyExists(componentTypeId))
			return nullptr;

		return registryMap1[componentTypeId];
	}

	void GameComponentRegistry::SortEntries()
	{
		
	}

} // namespace CE
