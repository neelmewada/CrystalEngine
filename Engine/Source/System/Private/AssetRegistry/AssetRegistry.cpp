#include "System.h"

namespace CE
{
	AssetRegistry* AssetRegistry::singleton = nullptr;

	AssetRegistry::AssetRegistry()
	{

	}

	AssetRegistry::~AssetRegistry()
	{
#if PAL_TRAIT_BUILD_EDITOR
		fileWatcher.RemoveWatcher(fileWatchID);
#endif
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

#if PAL_TRAIT_BUILD_EDITOR
			fileWatchID = fileWatcher.AddWatcher(projectAssetsPath, this, true);
			fileWatcher.Watch();
#endif
		}

		pathTreeCached = true;
	}

	void AssetRegistry::HandleFileAction(IO::WatchID watchId, IO::Path directory, const String& fileName, IO::FileAction fileAction, const String& oldFileName)
	{
		LockGuard<Mutex> guard{ mutex };

		// Watch for new/modified source assets
		IO::Path relative = IO::Path::GetRelative(directory, gProjectPath / "Game/Assets").GetString().Replace({ '\\' }, '/');
		u64 length = 0;
		auto filePath = directory / fileName;

		if (filePath.Exists() && !filePath.IsDirectory())
		{
			FileStream stream = FileStream(directory / fileName, Stream::Permissions::ReadOnly);
			stream.SetBinaryMode(true);

			length = stream.GetLength();

			stream.Close();
		}

		if (length > 0 || fileAction == IO::FileAction::Delete || fileAction == IO::FileAction::Moved)
		{
			CE_LOG(Info, All, "{} | Dir: {} | Name: {} | Old Name: {} | Length: {:#x}", fileAction, relative, fileName, oldFileName, length);
		}

	}

} // namespace CE
