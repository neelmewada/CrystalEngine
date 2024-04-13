#include "System.h"

namespace CE
{
	AssetRegistry* AssetRegistry::singleton = nullptr;

	static bool SortAssetData(AssetData* lhs, AssetData* rhs)
	{
		return String::NaturalCompare(lhs->packageName.GetLastComponent(), rhs->packageName.GetLastComponent());
	}

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

	void AssetRegistry::Shutdown()
	{
		
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
		return cachedPrimaryAssetsByParentPath[parentPath];
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

	Name AssetRegistry::GetPackagePath(Uuid packageUuid)
	{
		if (!cachedPrimaryAssetByPackageUuid.KeyExists(packageUuid))
			return Name();
		AssetData* assetData = cachedPrimaryAssetByPackageUuid[packageUuid];
		if (assetData == nullptr)
			return Name();
		return assetData->packageName;
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

		AssetData* assetData = nullptr;
		bool newEntry = false;
		int originalIndex = cachedPrimaryAssetsByParentPath[parentRelativePathStr].IndexOf([&](AssetData* data) -> bool { return data->packageName == load->GetPackageName(); });

		if (originalIndex >= 0)
		{
			assetData = cachedPrimaryAssetsByParentPath[parentRelativePathStr].At(originalIndex);
		}

		if (assetData == nullptr)
		{
			assetData = new AssetData();
			newEntry = true;
		}

		String sourceAssetRelativePath = "";
		if (!load->GetPrimaryObjectName().IsValid())
			load->LoadFully();

		Name primaryName = load->GetPrimaryObjectName();
		Name primaryTypeName = load->GetPrimaryObjectTypeName();
		assetData->packageName = load->GetPackageName();
		assetData->assetName = primaryName;
		assetData->assetClassPath = primaryTypeName;
		assetData->packageUuid = load->GetUuid();
		assetData->assetUuid = load->GetPrimaryObjectUuid();
		
#if PAL_TRAIT_BUILD_EDITOR
		// Source asset path relative to project assets directory
		sourceAssetRelativePath = load->GetPrimarySourceAssetRelativePath();
		assetData->sourceAssetPath = sourceAssetRelativePath;
#endif
		
		if (newEntry && relativePathStr.NonEmpty())
		{
			AddAssetEntry(relativePathStr, assetData);
		}

		load->RequestDestroy();

		if (listener != nullptr)
			listener->OnAssetImported(packageName, sourcePath);
		
		onAssetImported.Broadcast(packageName);
		onAssetRegistryModified.Broadcast();
	}

	void AssetRegistry::OnAssetDeleted(const Name& packageName)
	{
		IO::Path packagePath = Package::GetPackagePath(packageName);

		auto projectAssetsPath = gProjectPath / "Game/Assets";
		String relativePathStr = "";
		String parentRelativePathStr = "";

		if (IO::Path::IsSubDirectory(packagePath, projectAssetsPath))
		{
			relativePathStr = IO::Path::GetRelative(packagePath, gProjectPath).RemoveExtension().GetString().Replace({ '\\' }, '/');
			if (!relativePathStr.StartsWith("/"))
				relativePathStr = "/" + relativePathStr;

			parentRelativePathStr = IO::Path::GetRelative(packagePath, gProjectPath).GetParentPath().GetString().Replace({ '\\' }, '/');
			if (!parentRelativePathStr.StartsWith("/"))
				parentRelativePathStr = "/" + parentRelativePathStr;
		}
		else
		{
			return;
		}

		Name parentRelativePathName = parentRelativePathStr;
		Name relativePathName = relativePathStr;

		cachedPrimaryAssetsByParentPath.Remove(parentRelativePathName);

		cachedPathTree.RemovePath(relativePathName);
		cachedAssetsByPath.Remove(relativePathName);

		if (listener != nullptr)
			listener->OnAssetImported(packageName);

		onAssetRegistryModified.Broadcast();
	}

	void AssetRegistry::InitializeCache()
	{
		if (cacheInitialized)
			return;

		// Clear the path tree
		cachedPathTree.RemoveAll();
		directoryTree.RemoveAll();

		cachedPathTree.AddPath("/Game"); directoryTree.AddPath("/Game");
		cachedPathTree.AddPath("/Engine"); directoryTree.AddPath("/Engine");
#if PAL_TRAIT_BUILD_EDITOR
		//pathTree.AddPath("/Editor"); directoryTree.AddPath("/Editor");
#endif
		
		// Game assets
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
						if (load != nullptr)
						{
							AssetData* assetData = new AssetData();
							if (!load->GetPrimaryObjectName().IsValid())
								load->LoadFully();
							Name primaryName = load->GetPrimaryObjectName();
							Name primaryTypeName = load->GetPrimaryObjectTypeName();
							assetData->packageName = load->GetPackageName();
							assetData->assetName = primaryName;
							assetData->assetClassPath = primaryTypeName;
							assetData->packageUuid = load->GetUuid();
							assetData->assetUuid = load->GetPrimaryObjectUuid();
#if PAL_TRAIT_BUILD_EDITOR
							// Source asset path relative to project assets directory
							assetData->sourceAssetPath = load->GetPrimarySourceAssetRelativePath();
#endif
							load->RequestDestroy();
							load = nullptr;

							AddAssetEntry(relativePathStr, assetData);
						}
						else
						{
							CE_LOG(Error, All, "Failed to load asset metadata: {}", item);
						}
                    }
				});

#if PAL_TRAIT_BUILD_EDITOR
			fileWatchID = fileWatcher.AddWatcher(projectAssetsPath, this, true);
			fileWatcher.Watch();
#endif
		}

		// Engine assets
		if (gProjectPath.Exists() && (gProjectPath / "Engine/Assets").Exists())
		{
			auto projectAssetsPath = gProjectPath / "Engine/Assets";

			projectAssetsPath.RecursivelyIterateChildren([&](const IO::Path& item)
				{
					auto relativePath = IO::Path::GetRelative(item, gProjectPath);
					auto relativePathStr = relativePath.RemoveExtension().GetString().Replace({ '\\' }, '/');
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
						if (load != nullptr)
						{
							AssetData* assetData = new AssetData();
							if (!load->GetPrimaryObjectName().IsValid())
								load->LoadFully();
							Name primaryName = load->GetPrimaryObjectName();
							Name primaryTypeName = load->GetPrimaryObjectTypeName();
							assetData->packageName = load->GetPackageName();
							assetData->assetName = primaryName;
							assetData->assetClassPath = primaryTypeName;
							assetData->packageUuid = load->GetUuid();
							assetData->assetUuid = load->GetPrimaryObjectUuid();
#if PAL_TRAIT_BUILD_EDITOR
							// Source asset path relative to project assets directory
							assetData->sourceAssetPath = load->GetPrimarySourceAssetRelativePath();
#endif
							load->RequestDestroy();
							load = nullptr;

							AddAssetEntry(relativePathStr, assetData);
						}
						else
						{
							CE_LOG(Error, All, "Failed to load asset metadata: {}", item);
						}
					}
				});
		}

		auto launchDir = PlatformDirectories::GetLaunchDir();

		// Editor assets
		if ((launchDir / "Editor/Assets").Exists())
		{
			auto editorAssetsPath = launchDir / "Editor/Assets";

			editorAssetsPath.RecursivelyIterateChildren([&](const IO::Path& item)
				{
					auto relativePath = IO::Path::GetRelative(item, launchDir);
					auto relativePathStr = relativePath.RemoveExtension().GetString().Replace({ '\\' }, '/');
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
						if (load != nullptr)
						{
							AssetData* assetData = new AssetData();
							if (!load->GetPrimaryObjectName().IsValid())
								load->LoadFully();
							Name primaryName = load->GetPrimaryObjectName();
							Name primaryTypeName = load->GetPrimaryObjectTypeName();
							assetData->packageName = load->GetPackageName();
							assetData->assetName = primaryName;
							assetData->assetClassPath = primaryTypeName;
							assetData->packageUuid = load->GetUuid();
							assetData->assetUuid = load->GetPrimaryObjectUuid();
#if PAL_TRAIT_BUILD_EDITOR
							// Source asset path relative to project assets directory
							assetData->sourceAssetPath = load->GetPrimarySourceAssetRelativePath();
#endif
							load->RequestDestroy();
							load = nullptr;

							AddAssetEntry(relativePathStr, assetData);
						}
						else
						{
							CE_LOG(Error, All, "Failed to load asset metadata: {}", item);
						}
					}
				}
			);
		}

		cacheInitialized = true;
	}


	void AssetRegistry::AddAssetEntry(const Name& packageName, AssetData* assetData)
	{
		if (assetData == nullptr)
			return;

		allAssetDatas.Add(assetData);
		cachedPathTree.AddPath(packageName, assetData);
		cachedAssetsByPath[packageName].Add(assetData);
		cachedPrimaryAssetByPath[packageName] = assetData;
		cachedPrimaryAssetByPackageUuid[assetData->packageUuid] = assetData;

		String parentPathStr = IO::Path(packageName.GetString()).GetParentPath().GetString().Replace({ '\\' }, '/');
		Name parentPath = parentPathStr;
		
		cachedPrimaryAssetsByParentPath[parentPath].Add(assetData);
		cachedPrimaryAssetsByParentPath[parentPath].Sort(SortAssetData);

		if (assetData->sourceAssetPath.IsValid())
		{
			cachedAssetBySourcePath[assetData->sourceAssetPath] = assetData;
		}
	}

	void AssetRegistry::DeleteAssetEntry(const Name& packageName)
	{
		if (!cachedAssetsByPath.KeyExists(packageName))
			return;

		auto assetDatas = cachedAssetsByPath[packageName];

		cachedPathTree.RemovePath(packageName);
		cachedAssetsByPath.Remove(packageName);
		cachedPrimaryAssetByPath.Remove(packageName);
		
		String parentPathStr = IO::Path(packageName.GetString()).GetParentPath().GetString().Replace({ '\\' }, '/');
		Name parentPath = parentPathStr;

		cachedPrimaryAssetsByParentPath.Remove(parentPath);

		for (auto assetData : assetDatas)
		{
			if (assetData != nullptr)
			{
				Name sourcePath = assetData->sourceAssetPath;
				if (cachedAssetBySourcePath.KeyExists(sourcePath))
				{
					cachedAssetBySourcePath.Remove(sourcePath);
				}
				delete assetData;
			}
		}
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

		// FIX: Added delay to prevent skipping file Modified calls
		Thread::SleepFor(1);
	}

} // namespace CE
