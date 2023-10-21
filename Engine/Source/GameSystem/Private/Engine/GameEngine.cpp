#include "GameSystem.h"

namespace CE
{

	GameEngine::GameEngine()
	{
		if (!IsDefaultInstance())
			return;

		ClassType* assetManagerClass = Super::runtimeAssetManagerClass;
		if (assetManagerClass != nullptr)
		{
			if (!assetManagerClass->IsSubclassOf<AssetManager>())
				assetManagerClass = AssetManager::StaticType();
		}
		else
		{
			assetManagerClass = AssetManager::StaticType();
		}
		
		assetManager = CreateDefaultSubobject<AssetManager>(assetManagerClass, TEXT("AssetManager"));
	}

	GameEngine::~GameEngine()
	{

	}

} // namespace CE

