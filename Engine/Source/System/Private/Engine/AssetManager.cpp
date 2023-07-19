#include "System.h"

namespace CE
{
    
	AssetManager::AssetManager()
	{
		assetRegistry = CreateObject<AssetRegistry>(this, "AssetRegistry", OF_Transient);
		
		assetRegistry->CachePathTree();
	}

	AssetManager::~AssetManager()
	{

	}

	AssetManager* AssetManager::Get()
	{
		return nullptr;
	}

	AssetRegistry* AssetManager::GetRegistry()
	{
		return nullptr;
	}


} // namespace CE
