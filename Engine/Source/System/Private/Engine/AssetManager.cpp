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

			for (auto [uuid, bundle] : loadedAssetsByUuid)
			{
				if (bundle != nullptr)
				{
					bundle->Destroy();
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
		ZoneScoped;
		ZoneTextF(path.GetCString());

		Array<AssetData*> assetDatas = GetAssetsDataAtPath(path);
		if (assetDatas.IsEmpty())
			return {};

		Bundle* bundle = nullptr;

		loadedAssetsMutex.Lock();
		if (loadedAssetsByPath.KeyExists(path) && loadedAssetsByPath[path] != nullptr)
		{
			bundle = loadedAssetsByPath[path];
			loadedAssetsMutex.Unlock();
		}
		else
		{
			bundle = Bundle::LoadBundleFromDisk(nullptr, path, LOAD_Full);
			if (bundle == nullptr)
			{
				loadedAssetsMutex.Unlock();
				return {};
			}
			loadedAssetsByPath[path] = bundle;
			loadedAssetsByUuid[bundle->GetUuid()] = bundle;
			loadedAssetsMutex.Unlock();
		}
		
		if (!bundle)
			return {};

		Array<Asset*> assets{};

		if (classType == nullptr || !classType->IsSubclassOf<Asset>())
			classType = Asset::StaticType();

		for (auto assetData : assetDatas)
		{
			Object* object = bundle->LoadObject(assetData->assetUuid);
			if (object && object->IsOfType(classType))
			{
				assets.Add((Asset*)object);
			}
		}

		return assets;
	}

	void AssetManager::UnloadAsset(Asset* asset)
	{
		ZoneScoped;

		if (asset == nullptr)
			return;
		Bundle* bundle = asset->GetBundle();
		if (bundle == nullptr)
			return;

		LockGuard lock{ loadedAssetsMutex };

		loadedAssetsByPath.Remove(bundle->GetBundleName());
		loadedAssetsByUuid.Remove(bundle->GetBundleUuid());
		bundle->Destroy();
	}

	Asset* AssetManager::LoadAssetAtPath(const Name& path)
	{
		ZoneScoped;
		ZoneTextF(path.GetCString());

		AssetData* assetData = GetPrimaryAssetDataAtPath(path);
		if (!assetData)
			return nullptr;
		
		Bundle* bundle = nullptr;

		loadedAssetsMutex.Lock();
		if (loadedAssetsByPath.KeyExists(path) && loadedAssetsByPath[path] != nullptr)
		{
			bundle = loadedAssetsByPath[path];
			loadedAssetsMutex.Unlock();
		}
		else
		{
			bundle = Bundle::LoadBundleFromDisk(nullptr, path, LOAD_Full);
			if (bundle == nullptr)
			{
				loadedAssetsMutex.Unlock();
				return nullptr;
			}
			loadedAssetsByPath[path] = bundle;
			loadedAssetsByUuid[bundle->GetUuid()] = bundle;
			loadedAssetsMutex.Unlock();
		}
		
		if (!bundle)
			return nullptr;

		Object* object = bundle->LoadObject(bundle->GetPrimaryObjectUuid());
		if (!object || !object->IsOfType<Asset>())
			return nullptr;
		
		return (Asset*)object;
	}

} // namespace CE
