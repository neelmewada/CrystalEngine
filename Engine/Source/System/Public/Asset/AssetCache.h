#pragma once

namespace CE
{
	STRUCT()
	struct SYSTEM_API AssetCacheEntry
	{
		CE_STRUCT(AssetCacheEntry)
	public:

		void Release();

		FIELD()
		Name packagePath{};

		FIELD()
		Name sourcePath{}; // Source asset path, relative to the project directory

		FIELD()
		BinaryBlob thumbnailPNG;

	};

    CLASS()
	class SYSTEM_API AssetCache : public Object
	{
		CE_CLASS(AssetCache, Object)
	public:

		AssetCache();
		virtual ~AssetCache();

	protected:

		void OnAfterDeserialize() override;
		void OnBeforeSerialize() override;

		FIELD()
		Array<AssetCacheEntry> savedEntries{};

		
	};

} // namespace CE

#include "AssetCache.rtti.h"
