#include "EditorSystem.h"

namespace CE::Editor
{
	EditorAssetManager::EditorAssetManager()
	{
		// Texture 2D types
		registeredSourceExtensions[".png"] = 
			registeredSourceExtensions[".jpg"] = 
			GetStaticClass<Texture2D>();
	}

	EditorAssetManager::~EditorAssetManager()
	{
		
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
				// New file added or modified
				CE_LOG(Info, All, "File modified: {}", thisChange.currentPath);
			}
			else if (thisChange.fileAction == IO::FileAction::Moved)
			{
				CE_LOG(Info, All, "File moved: {}", thisChange.currentPath);
			}
		}

		sourceChanges.Clear();
    }

} // namespace CE::Editor
