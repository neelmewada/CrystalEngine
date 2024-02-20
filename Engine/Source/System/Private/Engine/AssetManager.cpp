#include "System.h"

namespace CE
{
    
	AssetManager::AssetManager()
	{
		assetRegistry = CreateDefaultSubobject<AssetRegistry>("AssetRegistry", OF_Transient);
	}

	AssetManager::~AssetManager()
	{
		for (auto [uuid, package] : loadedAssets)
		{
			package->Destroy();
		}
		loadedAssets.Clear();
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

	AssetData* AssetManager::GetPrimaryAssetDataAtPath(const Name& path)
	{
		if (!assetRegistry)
			return nullptr;
		return assetRegistry->GetPrimaryAssetByPath(path);
	}

	Asset* AssetManager::LoadAssetAtPath(const Name& path)
	{
		AssetData* assetData = GetPrimaryAssetDataAtPath(path);
		if (!assetData)
			return nullptr;

		Package* package = nullptr;

		if (loadedAssets.KeyExists(assetData->packageUuid))
			package = loadedAssets[assetData->packageUuid];

		if (!package)
			package = Package::LoadPackage(nullptr, path, LOAD_Default);

		if (!package)
			return nullptr;

		loadedAssets[assetData->packageUuid] = package;

		Object* object = package->LoadObject(assetData->assetUuid);
		if (!object || !object->IsOfType<Asset>())
			return nullptr;

		return (Asset*)object;
	}

} // namespace CE
