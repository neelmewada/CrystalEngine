#include "System.h"

namespace CE::Editor
{
	void AssetThumbnailCacheEntry::Release()
	{
		// Release the PNG memory
		thumbnailPNG.Free();
	}

	AssetCache::AssetCache()
	{

	}

	AssetCache::~AssetCache()
	{

	}

    void AssetCache::AddThumbnailCache(const Name& packagePath, const BinaryBlob& thumbnail)
    {

    }

	void AssetCache::OnAfterDeserialize()
	{
		Super::OnAfterDeserialize();

		cachedThumbnailsByPath.Clear();

		for (const auto& cacheEntry : thumbnailCache)
		{
			cachedThumbnailsByPath[cacheEntry.packagePath] = cacheEntry;
		}

		thumbnailCache.Clear();
	}

	void AssetCache::OnBeforeSerialize()
	{
		Super::OnBeforeSerialize();

		thumbnailCache.Clear();

		for (const auto& [packagePath, cacheEntry] : cachedThumbnailsByPath)
		{
			thumbnailCache.Add(cacheEntry);
		}
	}

} // namespace CE::Editor
