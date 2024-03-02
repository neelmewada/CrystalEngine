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
		{
			LockGuard<SharedMutex> lock{ loadedAssetsMutex };

			for (auto [uuid, package] : loadedAssetsByUuid)
			{
				if (package != nullptr)
				{
					package->Destroy();
				}
			}

			loadedAssetsByUuid.Clear();
			loadedAssetsByPath.Clear();
		}

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

	Array<AssetData*> AssetManager::GetAssetsDataAtPath(const Name& path)
	{
		if (!assetRegistry)
			return {};
		return assetRegistry->GetAssetsByPath(path);
	}

	Array<Asset*> AssetManager::LoadAssetsAtPath(const Name& path, SubClass<Asset> classType)
	{
		Array<AssetData*> assetDatas = GetAssetsDataAtPath(path);
		if (assetDatas.IsEmpty())
			return {};

		Package* package = nullptr;

		loadedAssetsMutex.Lock();
		if (loadedAssetsByPath.KeyExists(path) && loadedAssetsByPath[path] != nullptr)
		{
			package = loadedAssetsByPath[path];
			loadedAssetsMutex.Unlock();
		}
		else
		{
			package = Package::LoadPackage(nullptr, path, LOAD_Full);
			if (package == nullptr)
			{
				loadedAssetsMutex.Unlock();
				return {};
			}
			loadedAssetsByPath[path] = package;
			loadedAssetsByUuid[package->GetUuid()] = package;
			loadedAssetsMutex.Unlock();
		}
		
		if (!package)
			return {};

		Array<Asset*> assets{};

		if (classType == nullptr || !classType->IsSubclassOf<Asset>())
			classType = Asset::StaticType();

		for (auto assetData : assetDatas)
		{
			Object* object = package->LoadObject(assetData->assetUuid);
			if (object && object->IsOfType(classType))
			{
				assets.Add((Asset*)object);
			}
		}

		return assets;
	}

	void AssetManager::UnloadAsset(Asset* asset)
	{
		if (asset == nullptr)
			return;
		Package* package = asset->GetPackage();
		if (package == nullptr)
			return;

		loadedAssetsMutex.Lock();
		loadedAssetsByPath.Remove(package->GetPackageName());
		loadedAssetsByUuid.Remove(package->GetPackageUuid());
		package->Destroy();
		loadedAssetsMutex.Unlock();
	}

	Asset* AssetManager::LoadAssetAtPath(const Name& path)
	{
		AssetData* assetData = GetPrimaryAssetDataAtPath(path);
		if (!assetData)
			return nullptr;
		
		Package* package = nullptr;

		loadedAssetsMutex.Lock();
		if (loadedAssetsByPath.KeyExists(path) && loadedAssetsByPath[path] != nullptr)
		{
			package = loadedAssetsByPath[path];
			loadedAssetsMutex.Unlock();
		}
		else
		{
			package = Package::LoadPackage(nullptr, path, LOAD_Full);
			if (package == nullptr)
			{
				loadedAssetsMutex.Unlock();
				return nullptr;
			}
			loadedAssetsByPath[path] = package;
			loadedAssetsByUuid[package->GetUuid()] = package;
			loadedAssetsMutex.Unlock();
		}
		
		if (!package)
			return nullptr;

		Object* object = package->LoadObject(assetData->assetUuid);
		if (!object || !object->IsOfType<Asset>())
			return nullptr;
		
		return (Asset*)object;
	}

} // namespace CE
