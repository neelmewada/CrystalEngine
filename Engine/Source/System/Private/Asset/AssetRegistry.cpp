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

		for (auto assetData : allAssetDatas)
		{
			delete assetData;
		}
		allAssetDatas.Clear();

		cachedAssetsByPath.Clear();
	}

	AssetRegistry* AssetRegistry::Get()
	{
		return AssetManager::GetRegistry();
	}

	AssetData* AssetRegistry::GetAssetBySourcePath(const Name& sourcePath)
	{
		if (cachedAssetBySourcePath.KeyExists(sourcePath))
			return cachedAssetBySourcePath[sourcePath];
		return nullptr;
	}

	void AssetRegistry::OnAssetImported(const Name& packageName, const Name& sourcePath)
	{

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
					auto relativePath = IO::Path::GetRelative(item, gProjectPath);
					auto relativePathStr = relativePath.RemoveExtension().GetString().Replace({'\\'}, '/');
                    if (!relativePathStr.StartsWith("/"))
						relativePathStr = "/" + relativePathStr;
					if (item.IsDirectory()) // Folder
					{
                        if (!relativePathStr.IsEmpty())
						{
							cachedPathTree.AddPath(relativePathStr);
							directoryTree.AddPath(relativePathStr);
						}
					}
                    else if (item.GetExtension() == ".casset") // Product asset file
                    {
						Package* load = Package::LoadPackage(nullptr, item, LOAD_Default);
						AssetData* assetData = new AssetData();
						String sourceAssetRelativePath = "";
						if (load != nullptr)
						{
							if (!load->GetPrimaryObjectName().IsValid())
								load->LoadFully();
							Name primaryName = load->GetPrimaryObjectName();
							Name primaryTypeName = load->GetPrimaryObjectTypeName();
							assetData->packageName = load->GetPackageName();
							assetData->assetName = primaryName;
							assetData->assetClassPath = primaryTypeName;
#if PAL_TRAIT_BUILD_EDITOR
							// Source asset path relative to project assets directory
							sourceAssetRelativePath = load->GetPrimarySourceAssetRelativePath();
							if (!sourceAssetRelativePath.IsEmpty())
							{

							}
							assetData->sourceAssetPath = sourceAssetRelativePath;
#endif
							load->RequestDestroy();
							load = nullptr;
						}

						allAssetDatas.Add(assetData);
						cachedPathTree.AddPath(relativePathStr, assetData);
						cachedAssetsByPath[relativePathStr].Add(assetData);

						if (!sourceAssetRelativePath.IsEmpty())
						{
							cachedAssetBySourcePath[sourceAssetRelativePath] = assetData;
						}
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
        bool isDirectory = filePath.IsDirectory();
        bool isFile = !isDirectory;

		if (filePath.Exists() && !isDirectory)
		{
			FileStream stream = FileStream(directory / fileName, Stream::Permissions::ReadOnly);
			stream.SetBinaryMode(true);

			length = stream.GetLength();

			stream.Close();
		}
        
		if (isFile && (length > 0 || fileAction == IO::FileAction::Delete || fileAction == IO::FileAction::Moved || fileAction == IO::FileAction::Add))
		{
            if (length > 0 && (fileAction == IO::FileAction::Modified || fileAction == IO::FileAction::Add))
            {
				if (sourceChanges.IsEmpty() || 
					sourceChanges.Top().fileAction != IO::FileAction::Modified || 
					sourceChanges.Top().currentPath != filePath)
				{
					SourceAssetChange change{};
					change.fileAction = IO::FileAction::Modified;
					change.fileSize = length;
					change.currentPath = filePath;
					change.oldPath = "";
					sourceChanges.Add(change);
				}
            }
            else if (fileAction == IO::FileAction::Delete)
            {
                SourceAssetChange change{};
                change.fileAction = IO::FileAction::Delete;
                change.currentPath = filePath;
                change.fileSize = length;
                sourceChanges.Add(change);
            }
            else if (fileAction == IO::FileAction::Moved)
            {
                SourceAssetChange change{};
                change.fileAction = IO::FileAction::Moved;
                change.currentPath = filePath;
                change.oldPath = directory / oldFileName;
                change.fileSize = length;
                sourceChanges.Add(change);
            }
		}

		// Bug fix: Add delay to prevent skipping file Modified calls
		Thread::SleepFor(5);
	}

} // namespace CE
