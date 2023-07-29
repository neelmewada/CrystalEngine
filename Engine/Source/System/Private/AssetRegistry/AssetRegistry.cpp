#include "System.h"

namespace CE
{
	AssetRegistry* AssetRegistry::singleton = nullptr;

	AssetRegistry::AssetRegistry()
	{

	}

	AssetRegistry::~AssetRegistry()
	{
		
	}

	AssetRegistry* AssetRegistry::Get()
	{
		return AssetManager::GetRegistry();
	}

	void AssetRegistry::CachePathTree()
	{
		if (pathTreeCached)
			return;

		// Clear the path tree
		cachedPathTree.RemoveAll();
		directoryTree.RemoveAll();

		cachedPathTree.AddPath("/Game"); directoryTree.AddPath("/Game");
		cachedPathTree.AddPath("/Engine"); directoryTree.AddPath("/Engine");
#if PAL_TRAIT_BUILD_EDITOR
		//pathTree.AddPath("/Editor"); directoryTree.AddPath("/Editor");
#endif
		
		if (gProjectPath.Exists() && (gProjectPath / "Game/Assets").Exists())
		{
			auto projectAssetsPath = gProjectPath / "Game/Assets";

			projectAssetsPath.RecursivelyIterateChildren([&](const IO::Path& item)
				{
                    auto relativePath = IO::Path::GetRelative(item, gProjectPath / "Game/Assets").GetString();
                    if (!relativePath.StartsWith("/"))
                        relativePath = "/" + relativePath;
					if (item.IsDirectory()) // Folder
					{
                        if (!relativePath.IsEmpty())
						{
							cachedPathTree.AddPath("/Game" + relativePath);
							directoryTree.AddPath("/Game" + relativePath);
						}
					}
                    else if (relativePath.EndsWith(".casset")) // Product asset file
                    {
						cachedPathTree.AddPath("/Game" + relativePath);
                    }
				});
		}

		pathTreeCached = true;
	}

} // namespace CE
