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
                    auto relativePath = IO::Path::GetRelative(item, gProjectPath / "Game/Assets").GetString();
                    if (!relativePath.StartsWith("/"))
                        relativePath = "/" + relativePath;
					if (item.IsDirectory()) // Folder
					{
                        if (!relativePath.IsEmpty())
                            pathTree.AddPath("/Game" + relativePath);
					}
                    else if (relativePath.EndsWith(".casset")) // Asset file
                    {
                        pathTree.AddPath("/Game" + relativePath);
                    }
				});
		}

		pathTreeCached = true;
	}

} // namespace CE
