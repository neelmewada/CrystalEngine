#include "System.h"

namespace CE
{

	AssetManager* AssetManager::singleton = nullptr;
    
	AssetManager::AssetManager()
	{
		if (singleton == nullptr)
			singleton = this;

		assetRegistry = CreateObject<AssetRegistry>(this, "AssetRegistry", OF_Transient);
		
		assetRegistry->CachePathTree();
	}

	AssetManager::~AssetManager()
	{
		if (singleton == this)
			singleton = nullptr;
	}

	AssetManager* AssetManager::Get()
	{
		return singleton;
	}

	AssetRegistry* AssetManager::GetRegistry()
	{
		if (singleton == nullptr)
			return nullptr;
		return singleton->assetRegistry;
	}


} // namespace CE
