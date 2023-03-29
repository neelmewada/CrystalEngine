
#include "Asset/AssetManager.h"

#include "System.h"

namespace CE::Editor
{
	AssetManager::AssetManager()
	{

	}

	AssetManager::~AssetManager()
	{
		delete watcher;
		watcher = nullptr;
	}

	void AssetManager::LoadEditorAssetDatabase()
	{
		const auto& projectSettings = ProjectSettings::Get();
		auto engineDir = PlatformDirectories::GetEngineDir();
		auto engineAssetsDir = engineDir / "Assets";
		auto editorDir = PlatformDirectories::GetEditorDir();

#if PAL_TRAIT_BUILD_EDITOR
		auto gameDir = projectSettings.editorProjectDirectory / "Game";
#else
		auto gameDir = PlatformDirectories::GetGameDir();
#endif

		if (gameDir.IsEmpty() || !gameDir.Exists())
		{
			CE_LOG(Error, All, "Failed to load asset database! Invalid project path: {}", gameDir);
			return;
		}

		if (!engineDir.Exists() || !engineAssetsDir.Exists())
		{
			CE_LOG(Error, All, "Failed to load engine asset database! Invalid engine assets path: {}", engineDir);
		}

		if (!editorDir.Exists())
		{
			CE_LOG(Error, All, "Failed to load editor asset database! Invalid editor assets path: {}", editorDir);
		}

		auto root = AssetDatabase::Get().rootEntry;

		if (root == nullptr)
		{
			CE_LOG(Error, All, "Failed to load asset database! AssetDatabase::rootEntry is NULL.");
			return;
		}
		
		root->ClearChildren();

		// Load Engine Asset entries
		if (engineDir.Exists() && engineAssetsDir.Exists())
		{
			auto root = AssetDatabase::Get().rootEntry;

			root->children.Add({});

			auto engineAssetsRoot = &root->children.GetLast();
			engineAssetsRoot->name = "Engine";
			engineAssetsRoot->category = AssetDatabaseEntry::Category::EngineAssets;
			engineAssetsRoot->entryType = AssetDatabaseEntry::Type::Directory;
			engineAssetsRoot->virtualPath = "Engine/Assets";
			engineAssetsRoot->virtualRelativePath = engineAssetsRoot->virtualPath;

			engineAssetsDir.RecursivelyIterateChildren([&](const IO::Path& path)
			{
				auto relPath = IO::Path::GetRelative(path, engineAssetsDir);
				auto numComponents = std::distance(relPath.begin(), relPath.end());

				if (path.IsDirectory())
				{
					auto dirEntry = CE::AssetDatabaseEntry{};
					dirEntry.name = path.GetFilename().GetString();
					dirEntry.category = AssetDatabaseEntry::Category::EngineAssets;
					dirEntry.entryType = AssetDatabaseEntry::Type::Directory;

					CE::AssetDatabaseEntry* parentEntry = engineAssetsRoot;

					if (numComponents > 1)
					{
						for (auto it = relPath.begin(); it != relPath.end(); ++it)
						{
							auto str = String(it->string());
							for (auto& childEntry : parentEntry->children)
							{
								if (childEntry.virtualPath.GetFilename() == str)
								{
									parentEntry = &childEntry;
									break;
								}
							}
						}
					}

					dirEntry.virtualPath = parentEntry->virtualPath / relPath.GetFilename();
					dirEntry.virtualRelativePath = relPath.GetFilename();
					dirEntry.parent = parentEntry;
					parentEntry->children.Add(dirEntry);
				}
				else
				{
					auto copyPath = path;
					if (copyPath.GetExtension() == ".casset" || !copyPath.ReplaceExtension(".casset").Exists())
						return;

					auto relPathWithFixedExtension = relPath.ReplaceExtension(".casset");

					auto assetEntry = CE::AssetDatabaseEntry{};
					assetEntry.name = path.GetFilename().RemoveExtension().GetString();
					assetEntry.extension = copyPath.GetExtension().GetString();
					assetEntry.category = AssetDatabaseEntry::Category::EngineAssets;
					assetEntry.entryType = AssetDatabaseEntry::Type::Asset;

					CE::AssetDatabaseEntry* parentEntry = engineAssetsRoot;

					if (numComponents > 1)
					{
						for (auto it = relPath.begin(); it != relPath.end(); ++it)
						{
							auto str = String(it->string());
							for (auto& childEntry : parentEntry->children)
							{
								if (childEntry.virtualPath.GetFilename() == str)
								{
									parentEntry = &childEntry;
									break;
								}
							}
						}
					}

					assetEntry.virtualPath = parentEntry->virtualPath / relPathWithFixedExtension.GetFilename();
					assetEntry.virtualRelativePath = relPathWithFixedExtension.GetFilename();
					assetEntry.parent = parentEntry;
					parentEntry->children.Add(assetEntry);
				}
			});
		}

		if (watcher == nullptr)
		{
			watcher = new IO::FileWatcher();
			gameAssetsWatch = watcher->AddWatcher(gameDir, this, true);

			watcher->Watch();
		}
	}

	void AssetManager::HandleFileAction(IO::WatchID watchId, IO::Path directory, String fileName, IO::FileAction fileAction, String oldFileName)
	{

	}

} // namespace CE::Editor

