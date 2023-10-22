#include "GameSystem.h"

namespace CE
{

	GameEngine::GameEngine()
	{
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

    void GameEngine::Initialize()
    {
		Super::Initialize();

		gameInstance = CreateObject<GameInstance>(this, TEXT("GameInstance"), OF_Transient);
		gameInstances.Add(gameInstance);
    }

	void GameEngine::Shutdown()
	{
		Super::Shutdown();

		gameInstances.Remove(gameInstance);
		gameInstance->Destroy();
		gameInstance = nullptr;
	}

} // namespace CE

