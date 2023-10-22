#include "System.h"

namespace CE
{
    
	AssetManager::AssetManager()
	{
		assetRegistry = CreateDefaultSubobject<AssetRegistry>("AssetRegistry", OF_Transient);
	}

	AssetManager::~AssetManager()
	{

	}

	AssetManager* AssetManager::Get()
	{
		if (gEngine == nullptr)
			return nullptr;
		return gEngine->GetAssetManager();
	}

	AssetRegistry* AssetManager::GetRegistry()
	{
		if (Self::Get() == nullptr)
			return nullptr;
		return Self::Get()->assetRegistry;
	}

	void AssetManager::Initialize()
	{
		// Cache asset paths & load saved cache
		assetRegistry->InitializeCache();
	}

	void AssetManager::Shutdown()
	{
		assetRegistry->Shutdown();
	}

	void AssetManager::Tick(f32 deltaTime)
	{
		
	}

} // namespace CE
