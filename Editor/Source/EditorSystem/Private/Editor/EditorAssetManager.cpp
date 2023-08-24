#include "EditorSystem.h"

namespace CE::Editor
{
	EditorAssetManager::EditorAssetManager()
	{
		
	}

	EditorAssetManager::~EditorAssetManager()
	{
		
	}

	EditorAssetManager* EditorAssetManager::Get()
	{
		return (EditorAssetManager*)AssetManager::Get();
	}

	void EditorAssetManager::Initialize()
	{
		Super::Initialize();

		cachePackage = Package::LoadPackage(nullptr, Name("/Temp/AssetCache"));
		if (cachePackage == nullptr)
			cachePackage = CreateObject<Package>(nullptr, "/Temp/AssetCache");
		cache = cachePackage->LoadObject<AssetCache>();
		if (cache == nullptr)
			cache = CreateObject<AssetCache>(cachePackage, "Cache");

		assetRegistry->listener = this;
	}

	void EditorAssetManager::Shutdown()
	{
		assetRegistry->listener = nullptr;

		Package::SavePackage(cachePackage, nullptr);
		cachePackage->Destroy();
		cache = nullptr;
		cachePackage = nullptr;

		Super::Shutdown();
	}

    void EditorAssetManager::Tick(f32 deltaTime)
    {
		Super::Tick(deltaTime);

		auto& sourceChanges = assetRegistry->sourceChanges;

		for (int i = 0; i < sourceChanges.GetSize(); i++)
		{
			AssetRegistry::SourceAssetChange thisChange = sourceChanges[i];
			AssetRegistry::SourceAssetChange nextChange = {};
			bool nextFound = false;
			if (i < sourceChanges.GetSize() - 1 && sourceChanges[i + 1].fileAction == IO::FileAction::Modified && thisChange.fileAction == IO::FileAction::Delete)
			{
				nextFound = true;
				nextChange = sourceChanges[i + 1];
				i++;
			}

			IO::Path relative = IO::Path::GetRelative(thisChange.currentPath, gProjectPath / "Game/Assets").GetString().Replace({ '\\' }, '/');
			String extension = thisChange.currentPath.GetExtension().GetString();
			String pathNameWithoutExtension = relative.RemoveExtension().GetString();
			if (!pathNameWithoutExtension.StartsWith("/"))
				pathNameWithoutExtension = "/" + pathNameWithoutExtension;

			if (thisChange.fileAction == IO::FileAction::Delete && nextFound &&
				nextChange.fileAction == IO::FileAction::Modified &&
				nextChange.currentPath.GetFilename() == thisChange.currentPath.GetFilename()) // File moved
			{
				//CE_LOG(Info, All, "File moved: {}", nextChange.currentPath);
			}
			else if (thisChange.fileAction == IO::FileAction::Delete) // File deleted
			{
				if (extension == ".casset") // Product asset deleted
				{
					assetRegistry->OnAssetDeleted(pathNameWithoutExtension);
				}
			}
			else if (thisChange.fileAction == IO::FileAction::Modified) // Product asset
			{
				if (extension == ".casset")
				{
					// Do nothing if product asset is modified
				}
				else
				{
					// Source asset
					AssetDefinition* assetDef = AssetDefinitionRegistry::Get()->FindAssetDefinitionForSourceAssetExtension(extension);
					if (assetDef != nullptr && !sourceAssetsToImport.Exists(thisChange.currentPath))
					{
						sourceAssetsToImport.Add(thisChange.currentPath);

						waitToImportSourceAssets = 3.0f;
					}
;				}

				// New file added or current file modified
			}
			else if (thisChange.fileAction == IO::FileAction::Moved) // File moved (undefined behaviour)
			{

			}
		}

		sourceChanges.Clear();

		if (!sourceAssetsToImport.IsEmpty())
		{
			if (waitToImportSourceAssets > 0)
			{
				waitToImportSourceAssets -= deltaTime;
			}
			else
			{
				waitToImportSourceAssets = 0;
				// Import assets after delay
				ImportSourceAssets();
			}
		}

		if (!mainThreadQueue.IsEmpty())
		{
			mainThreadQueue.GetFront().InvokeIfValid();
			mainThreadQueue.PopFront();
		}

		for (int i = importersInProgress.GetSize() - 1; i >= 0; i--)
		{
			if (!importersInProgress[i]->IsImportInProgress())
			{
				importersInProgress[i]->Destroy();
				importersInProgress.RemoveAt(i);
			}
		}
    }

	void EditorAssetManager::ImportSourceAssets()
	{
		HashMap<AssetImporter*, Array<IO::Path>> sourcePathsToProcess{};
		HashMap<AssetImporter*, Array<IO::Path>> productPaths{};

		for (auto sourcePath : sourceAssetsToImport)
		{
			String extension = sourcePath.GetExtension().GetString();

			AssetDefinition* assetDef = AssetDefinitionRegistry::Get()->FindAssetDefinitionForSourceAssetExtension(extension);
			if (assetDef == nullptr)
				continue;
			ClassType* assetImporterClass = assetDef->GetAssetImporterClass();
			if (assetImporterClass == nullptr)
				continue;
			AssetImporter* assetImporter = CreateObject<AssetImporter>(this, "AssetImporter", OF_NoFlags, assetImporterClass);
			if (assetImporter == nullptr)
				continue;

			IO::Path productAssetPath = sourcePath.ReplaceExtension(".casset");

			// Relative path to source asset
			String sourcePathName = "";
			if (IO::Path::IsSubDirectory(sourcePath, gProjectPath / "Game/Assets"))
			{
				sourcePathName = IO::Path::GetRelative(sourcePath, gProjectPath).GetString().Replace({ '\\' }, '/');
				if (!sourcePathName.StartsWith("/"))
					sourcePathName = "/" + sourcePathName;

				auto assetBySourcePath = assetRegistry->GetAssetBySourcePath(sourcePathName);
				if (assetBySourcePath != nullptr)
				{
					Name assetPackageName = assetBySourcePath->packageName;
					auto packagePath = Package::GetPackagePath(assetPackageName);
					if (packagePath.Exists())
					{
						productAssetPath = packagePath;
					}
					else
					{
						assetRegistry->cachedAssetBySourcePath.Remove(sourcePathName);
					}
				}
			}

			sourcePathsToProcess[assetImporter].Add(sourcePath);
			productPaths[assetImporter].Add(productAssetPath);

			Object::Bind(assetImporter, MEMBER_FUNCTION(AssetImporter, OnAssetImportResult), 
				[=](bool success, IO::Path sourcePath, Name packageName) -> void
				{
					auto sourcePathString = sourcePath.GetString().Replace({ '\\' }, '/');
					if (!sourcePathString.StartsWith("/"))
						sourcePathString = "/" + sourcePathString;

					mutex.Lock();
					mainThreadQueue.PushBack([=]()
						{
							if (success)
							{
								assetRegistry->OnAssetImported(packageName, sourcePathString);
								//AssetRegistry::Get()->OnAssetImported(packageName, sourcePathString);
								BinaryBlob thumbnail{};
								if (assetImporter->GenerateThumbnail(packageName, thumbnail))
								{
									
								}
								assetImporter->numThumbnailsInProgress--;
								thumbnail.Free();
							}
						});
					mutex.Unlock();
				});
		}

		for (auto [importer, sourcePaths] : sourcePathsToProcess)
		{
			auto outPaths = productPaths[importer];

			importer->ImportSourceAssets(sourcePaths, outPaths);
			importersInProgress.Add(importer);
		}

		sourceAssetsToImport.Clear();
		waitToImportSourceAssets = 0;
	}

	void EditorAssetManager::OnAssetImportResult(bool success, IO::Path sourcePath, Name packageName)
	{
		auto sourcePathString = sourcePath.GetString().Replace({ '\\' }, '/');
		if (!sourcePathString.StartsWith("/"))
			sourcePathString = "/" + sourcePathString;
		
		mutex.Lock();
		mainThreadQueue.PushBack([=]()
			{
				if (success)
				{
					AssetRegistry::Get()->OnAssetImported(packageName, sourcePathString);
				}
			});
		mutex.Unlock();
	}

} // namespace CE::Editor
