#include "System.h"

namespace CE
{
	AssetRegistry* AssetRegistry::singleton = nullptr;

	AssetRegistry::AssetRegistry()
	{
		if (singleton == nullptr)
			singleton = this;

		
	}

	AssetRegistry::~AssetRegistry()
	{
		if (singleton == this)
			singleton = nullptr;
		

	}

	void AssetRegistry::CachePathTree()
	{
		if (pathTreeCached)
			return;

		// Clear the path tree
		pathTree.RemoveAll();

		pathTree.AddPath("/Game");
		pathTree.AddPath("/Engine");
#if PAL_TRAIT_BUILD_EDITOR
		pathTree.AddPath("/Editor");
#endif

		if (gProjectPath.Exists() && (gProjectPath / "Game/Assets").Exists())
		{
			auto projectAssetsPath = gProjectPath / "Game/Assets";

			projectAssetsPath.RecursivelyIterateChildren([&](const IO::Path& item)
				{
					if (item.IsDirectory())
					{
						CE_LOG(Info, All, "Dir: {}", item);
					}
				});
		}

		pathTreeCached = true;
	}

} // namespace CE
