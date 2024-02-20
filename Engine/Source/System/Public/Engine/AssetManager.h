#pragma once

namespace CE
{

	CLASS()
	class SYSTEM_API AssetManager : public CE::Object
	{
		CE_CLASS(AssetManager, CE::Object)
	public:

		AssetManager();
		virtual ~AssetManager();

		static AssetManager* Get();

		static AssetRegistry* GetRegistry();

		virtual void Initialize();
		virtual void Shutdown();

		virtual void Tick(f32 deltaTime);

		AssetData* GetPrimaryAssetDataAtPath(const Name& path);

		Asset* LoadAssetAtPath(const Name& path);

		template<typename TAsset> requires TIsBaseClassOf<Asset, TAsset>::Value
		inline TAsset* LoadAssetAtPath(const Name& path)
		{
			Asset* asset = LoadAssetAtPath(path);
			if (!asset)
				return nullptr;
			return Object::CastTo<TAsset>(asset);
		}

	protected:

		HashMap<Uuid, Package*> loadedAssets{};

		FIELD()
		AssetRegistry* assetRegistry = nullptr;

		friend class Engine;
	};
    
} // namespace CE

#include "AssetManager.rtti.h"
