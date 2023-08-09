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

	AssetData* AssetRegistry::GetPrimaryAssetByPath(const Name& path)
	{
		return cachedPrimaryAssetByPath[path];
	}

	Array<AssetData*> AssetRegistry::GetAssetsByPath(const Name& path)
	{
		return cachedAssetsByPath[path];
	}

	AssetData* AssetRegistry::GetAssetBySourcePath(const Name& sourcePath)
	{
		if (cachedAssetBySourcePath.KeyExists(sourcePath))
			return cachedAssetBySourcePath[sourcePath];
		return nullptr;
	}

	Array<AssetData*> AssetRegistry::GetPrimaryAssetsInSubPath(const Name& parentPath)
	{
		return cachedPrimaryAssetByParentPath[parentPath];
	}

	Array<String> AssetRegistry::GetSubDirectoriesAtPath(const Name& path)
	{
		Array<String> result{};

		auto directoryNode = directoryTree.GetNode(path);
		if (directoryNode == nullptr)
			return result;

		for (const auto subDirectory : directoryNode->children)
		{
			result.Add(subDirectory->name.GetString());
		}

		return result;
	}

	PathTreeNode* AssetRegistry::GetDirectoryNode(const Name& path)
	{
		return directoryTree.GetNode(path);
	}

	void AssetRegistry::OnAssetImported(const Name& packageName, const Name& sourcePath)
	{
		IO::Path packagePath = Package::GetPackagePath(packageName);
		Package* load = Package::LoadPackage(nullptr, packageName, LOAD_Default);
		if (load == nullptr)
			return;
		auto projectAssetsPath = gProjectPath / "Game/Assets";
		String relativePathStr = "";
		String parentRelativePathStr = "";

		if (IO::Path::IsSubDirectory(packagePath, projectAssetsPath))
		{
			relativePathStr = IO::Path::GetRelative(packagePath, gProjectPath).RemoveExtension().GetString().Replace({'\\'}, '/');
			if (!relativePathStr.StartsWith("/"))
				relativePathStr = "/" + relativePathStr;

			parentRelativePathStr = IO::Path::GetRelative(packagePath, gProjectPath).GetParentPath().GetString().Replace({ '\\' }, '/');
			if (!parentRelativePathStr.StartsWith("/"))
				parentRelativePathStr = "/" + parentRelativePathStr;
		}

		AssetData* assetData = new AssetData();
		String sourceAssetRelativePath = "";
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
		assetData->sourceAssetPath = sourceAssetRelativePath;
#endif

		allAssetDatas.Add(assetData);
		if (relativePathStr.NonEmpty())
		{
			directoryTree.AddPath(parentRelativePathStr);

			cachedPathTree.AddPath(relativePathStr, assetData);
			cachedAssetsByPath[relativePathStr].Add(assetData);

			cachedPrimaryAssetByParentPath[parentRelativePathStr].Add(assetData);
		}

		if (!sourceAssetRelativePath.IsEmpty())
		{
			cachedAssetBySourcePath[sourceAssetRelativePath] = assetData;
		}

		load->RequestDestroy();

		onAssetRegistryModified.Broadcast();
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

					String parentRelativePathStr = relativePath.GetParentPath().GetString().Replace({ '\\' }, '/');
					if (!parentRelativePathStr.StartsWith("/"))
						parentRelativePathStr = "/" + parentRelativePathStr;

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
						cachedPrimaryAssetByPath[relativePathStr] = assetData;

						cachedPrimaryAssetByParentPath[parentRelativePathStr].Add(assetData);

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
		mutex.Lock();

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
					//sourceChanges.Top().fileAction != IO::FileAction::Modified || 
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

		mutex.Unlock();

		// Bug fix: Add delay to prevent skipping file Modified calls
		Thread::SleepFor(1);
	}

} // namespace CE