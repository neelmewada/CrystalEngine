#include "System.h"

namespace CE
{
	AssetCache::AssetCache()
	{

	}

	AssetCache::~AssetCache()
	{

	}

	void AssetCache::OnAfterDeserialize()
	{
		Super::OnAfterDeserialize();

	}

	void AssetCache::OnBeforeSerialize()
	{
		Super::OnBeforeSerialize();

	}

	void AssetCacheEntry::Release()
	{
		thumbnailPNG.Free();
	}

} // namespace CE
