#pragma once

namespace CE
{
	// TODO: Implement CWidgetResourceLoader in AssetManager

	CLASS()
	class SYSTEM_API AssetManager : public Object
	{
		CE_CLASS(AssetManager, Object)
	public:

		AssetManager();
		virtual ~AssetManager();

		static AssetManager* Get();

		static AssetRegistry* GetRegistry();

		virtual void Initialize();
		virtual void Shutdown();

		virtual void Tick(f32 deltaTime);

		AssetData* GetPrimaryAssetDataAtPath(const Name& path);
		Array<AssetData*> GetAssetsDataAtPath(const Name& path);

		Asset* LoadAssetAtPath(const Name& path);

		Array<Asset*> LoadAssetsAtPath(const Name& path, SubClass<Asset> classType = Asset::StaticType());

		template<typename TAsset> requires TIsBaseClassOf<Asset, TAsset>::Value
		inline Array<TAsset*> LoadAssetsAtPath(const Name& path)
		{
			Array<Asset*> assets = LoadAssetsAtPath(path, TAsset::StaticType());
			Array<TAsset*> outAssets{};
			for (auto asset : assets)
			{
				TAsset* cast = Object::CastTo<TAsset>(asset);
				if (cast)
				{
					outAssets.Add(cast);
				}
			}
			return outAssets;
		}

		template<typename TAsset> requires TIsBaseClassOf<Asset, TAsset>::Value
		inline TAsset* LoadAssetAtPath(const Name& path)
		{
			Asset* asset = LoadAssetAtPath(path);
			if (!asset)
				return nullptr;
			return Object::CastTo<TAsset>(asset);
		}

		void UnloadAsset(Asset* asset);

	protected:

		FIELD()
		AssetRegistry* assetRegistry = nullptr;

		SharedMutex loadedAssetsMutex{};
		HashMap<Name, Bundle*> loadedAssetsByPath{};
		HashMap<Uuid, Bundle*> loadedAssetsByUuid{};

		friend class Engine;
	};
    
} // namespace CE

#include "AssetManager.rtti.h"
