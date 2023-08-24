#pragma once

namespace CE::Editor
{
	STRUCT()
	struct EDITORSYSTEM_API AssetThumbnailCacheEntry
	{
		CE_STRUCT(AssetThumbnailCacheEntry)
	public:

		void Release();

		FIELD()
		Name packagePath{};

		FIELD()
		BinaryBlob thumbnailPNG{};
	};

    CLASS()
	class EDITORSYSTEM_API AssetCache : public Object
	{
		CE_CLASS(AssetCache, Object)
	public:

		AssetCache();
		virtual ~AssetCache();

		void AddThumbnailCache(const Name& packagePath, const BinaryBlob& thumbnail);

	protected:

		void OnAfterDeserialize() override;
		void OnBeforeSerialize() override;

		FIELD()
		Array<AssetThumbnailCacheEntry> thumbnailCache{};

		HashMap<Name, AssetThumbnailCacheEntry> cachedThumbnailsByPath;
	};

} // namespace CE::Editor

#include "AssetCache.rtti.h"
