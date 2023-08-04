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
				CE_LOG(Info, All, "File moved: {}", nextChange.currentPath);
			}
			else if (thisChange.fileAction == IO::FileAction::Delete) // File deleted
			{
				bool isDirectory = thisChange.currentPath.GetExtension().IsEmpty();
				CE_LOG(Info, All, "File deleted: {} | {}", thisChange.currentPath, isDirectory);
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
					if (assetDef != nullptr)
					{
						String relativePath = pathNameWithoutExtension + extension;
						sourceAssetsToImport.Add((gProjectPath / "Game/Assets") / relativePath);

						waitToImportSourceAssets = 1.0f;
					}
;				}

				// New file added or current file modified
				CE_LOG(Info, All, "File modified: {}", thisChange.currentPath);
			}
			else if (thisChange.fileAction == IO::FileAction::Moved)
			{
				CE_LOG(Info, All, "File moved: {}", thisChange.currentPath);
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

	bool ImportSourceAssetAsync(EditorAssetManager* self, IO::Path sourceAssetPath, AssetImporter* importer)
	{
		String extension = sourceAssetPath.GetExtension().GetString();

		AssetDefinition* assetDef = AssetDefinitionRegistry::Get()->FindAssetDefinitionForSourceAssetExtension(extension);
		if (assetDef == nullptr)
		{
			self->numAssetsBeingImported--;
			if (importer != nullptr)
			{
				self->mutex.Lock();
				importer->RequestDestroy();
				self->mutex.Unlock();
			}
			return false;
		}

		IO::Path fullPath = sourceAssetPath;
		if (!fullPath.Exists())
		{
			self->numAssetsBeingImported--;
			if (importer != nullptr)
			{
				self->mutex.Lock();
				importer->RequestDestroy();
				self->mutex.Unlock();
			}
			return false;
		}

		IO::Path productAssetPath = fullPath.ReplaceExtension(".casset");

		if (importer == nullptr)
		{
			self->numAssetsBeingImported--;
			return false;
		}

		Name outPackageName = importer->ImportSourceAsset(fullPath, productAssetPath);

		self->numAssetsBeingImported--;
		if (importer != nullptr)
		{
			self->mutex.Lock();
			if (outPackageName.IsValid())
				self->recentlyProcessedPackageNames.Add(outPackageName);
			importer->RequestDestroy();
			self->mutex.Unlock();
		}
		return outPackageName.IsValid();
	}

	void EditorAssetManager::ImportSourceAssets()
	{
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

			numAssetsBeingImported++;
			auto run = Thread([=]
				{
					ImportSourceAssetAsync(this, sourcePath, assetImporter);
				});
		}

		sourceAssetsToImport.Clear();
		waitToImportSourceAssets = 0;
	}

} // namespace CE::Editor
