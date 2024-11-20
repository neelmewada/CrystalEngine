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
			LockGuard lock{ loadedAssetsMutex };

			for (auto [uuid, bundle] : loadedAssetsByUuid)
			{
				if (bundle != nullptr)
				{
					bundle->BeginDestroy();
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

	Array<Ref<Asset>> AssetManager::LoadAssetsAtPath(const Name& path, SubClass<Asset> classType)
	{
		ZoneScoped;
		ZoneTextF(path.GetCString());

		Array<AssetData*> assetDatas = GetAssetsDataAtPath(path);
		if (assetDatas.IsEmpty())
			return {};

		Ref<Bundle> bundle = nullptr;

		loadedAssetsMutex.Lock();
		if (loadedAssetsByPath.KeyExists(path) && loadedAssetsByPath[path] != nullptr)
		{
			bundle = loadedAssetsByPath[path];
			loadedAssetsMutex.Unlock();
		}
		else
		{
			LoadBundleArgs args{
				.loadFully = true,
				.forceReload = false
			};

			bundle = Bundle::LoadBundle(this, path, args);
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

		Array<Ref<Asset>> assets{};

		if (classType == nullptr || !classType->IsSubclassOf<Asset>())
			classType = Asset::StaticType();

		for (auto assetData : assetDatas)
		{
			Ref<Object> object = bundle->LoadObject(assetData->assetUuid);
			if (object.IsValid() && object->IsOfType(classType))
			{
				assets.Add((Ref<Asset>)object);
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

		loadedAssetsByPath.Remove(bundle->GetName());
		loadedAssetsByUuid.Remove(bundle->GetUuid());
		bundle->BeginDestroy();
	}

	Ref<Asset> AssetManager::LoadAssetAtPath(const Name& path)
	{
		ZoneScoped;
		ZoneTextF(path.GetCString());

		AssetData* assetData = GetPrimaryAssetDataAtPath(path);
		if (!assetData)
			return nullptr;
		
		Ref<Bundle> bundle = nullptr;

		loadedAssetsMutex.Lock();
		if (loadedAssetsByPath.KeyExists(path) && loadedAssetsByPath[path] != nullptr)
		{
			bundle = loadedAssetsByPath[path];
			loadedAssetsMutex.Unlock();
		}
		else
		{
			LoadBundleArgs args{
				.loadFully = true
			};

			bundle = Bundle::LoadBundle(this, path, args);
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

		auto primaryObject = bundle->GetPrimaryObjectData();

		Ref<Object> object = bundle->LoadObject(primaryObject.uuid);
		if (!object || !object->IsOfType<Asset>())
			return nullptr;
		
		return (Ref<Asset>)object;
	}

	RHI::Texture* AssetManager::LoadTextureAtPath(const Name& path)
	{
		Ref<CE::Texture2D> texture = LoadAssetAtPath<CE::Texture2D>(path);
		if (!texture)
			return nullptr;

		RPI::Texture* rpiTexture = texture->GetRpiTexture();
		if (!rpiTexture)
			return nullptr;

		return rpiTexture->GetRhiTexture();
	}
} // namespace CE
