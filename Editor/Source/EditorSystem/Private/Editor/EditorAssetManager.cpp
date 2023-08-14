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
	}

	void EditorAssetManager::Shutdown()
	{
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
				nextChange.currentPath.GetFilename() == thisChange.currentPath.GetFilename())
			{
				//CE_LOG(Info, All, "File moved: {}", nextChange.currentPath);
			}
			else if (thisChange.fileAction == IO::FileAction::Delete) // File deleted
			{
				bool isDirectory = thisChange.currentPath.GetExtension().IsEmpty();
				//CE_LOG(Info, All, "File deleted: {} | {}", thisChange.currentPath, isDirectory);
			}
			else if (thisChange.fileAction == IO::FileAction::Modified)
			{
				if (extension == ".casset") // Product asset
				{
					
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
				//CE_LOG(Info, All, "File modified: {}", thisChange.currentPath);
			}
			else if (thisChange.fileAction == IO::FileAction::Moved)
			{
				//CE_LOG(Info, All, "File moved: {}", thisChange.currentPath);
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
    }

	bool EditorAssetManager::ImportSourceAsset(IO::Path sourceAssetPath, AssetImporter* importer)
	{
		String extension = sourceAssetPath.GetExtension().GetString();

		AssetDefinition* assetDef = AssetDefinitionRegistry::Get()->FindAssetDefinitionForSourceAssetExtension(extension);
		if (assetDef == nullptr)
		{
			numAssetsBeingImported--;
			if (importer != nullptr)
			{
				importer->RequestDestroy();
			}
			return false;
		}

		IO::Path sourcePath = sourceAssetPath;
		if (!sourcePath.Exists())
		{
			numAssetsBeingImported--;
			if (importer != nullptr)
			{
				importer->RequestDestroy();
			}
			return false;
		}

		IO::Path productAssetPath = sourcePath.ReplaceExtension(".casset");

		if (importer == nullptr)
		{
			numAssetsBeingImported--;
			return false;
		}

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

		CE_LOG(Info, All, "Processing asset: {}", sourcePath);
		return false;

		Name outPackageName = importer->ImportSourceAsset(sourcePath, productAssetPath);

		if (outPackageName.IsValid()) // Import success
		{
			assetRegistry->OnAssetImported(outPackageName, sourcePathName);
		}

		numAssetsBeingImported--;
		
		if (importer != nullptr)
		{
			if (outPackageName.IsValid())
				recentlyProcessedPackageNames.Add(outPackageName);
			importer->RequestDestroy();
		}
		return outPackageName.IsValid();
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

			// TODO: implement async
			//auto jobs = assetImporter->ImportSourceAssets({ sourcePath }, )

			sourcePathsToProcess[assetImporter].Add(sourcePath);
			productPaths[assetImporter].Add(productAssetPath);
		}

		for (auto [importer, sourcePaths] : sourcePathsToProcess)
		{
			auto outPaths = productPaths[importer];

			importersInProgress.Add(importer);
			importer->ImportSourceAssets(sourcePaths, outPaths);
		}

		sourceAssetsToImport.Clear();
		waitToImportSourceAssets = 0;
	}

} // namespace CE::Editor
