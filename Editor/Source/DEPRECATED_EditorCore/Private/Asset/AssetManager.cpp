
#include "System.h"
#include "EditorCore.h"
#include "Asset/AssetManager.h"


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

		auto gameDir = projectSettings.editorProjectDirectory / "Game";

		auto gameAssetsDir = gameDir / "Assets";

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

		// *********************************************
		// Load Game/Project Asset entries

		if (gameDir.Exists() && gameAssetsDir.Exists())
		{
			root->children.Add(new AssetDatabaseEntry);

			auto gameAssetsRoot = root->children.GetLast();
			gameAssetsRoot->name = "Game";
			gameAssetsRoot->category = AssetDatabaseEntry::Category::GameAssets;
			gameAssetsRoot->entryType = AssetDatabaseEntry::Type::Directory;
			gameAssetsRoot->virtualRelativePath = "Game/Assets";
			gameAssetsRoot->parent = root;

			gameAssetsDir.RecursivelyIterateChildren([=](const IO::Path& path)
			{
				auto relPath = IO::Path::GetRelative(path, gameAssetsDir);
				auto numComponents = std::distance(relPath.begin(), relPath.end());

				if (path.IsDirectory())
				{
					CE::AssetDatabaseEntry* parentEntry = gameAssetsRoot;

					// Find the parent entry to add this directory to
					if (numComponents > 1)
					{
						for (auto it = relPath.begin(); it != relPath.end(); ++it)
						{
							auto str = String(it->string());
							for (auto& childEntry : parentEntry->children)
							{
								if (childEntry->virtualRelativePath.GetFilename() == str)
								{
									parentEntry = childEntry;
									break;
								}
							}
						}
					}

					parentEntry->children.Add(new AssetDatabaseEntry);
					auto dirEntry = parentEntry->children.GetLast();

					dirEntry->name = path.GetFilename().GetString();
					dirEntry->category = AssetDatabaseEntry::Category::GameAssets;
					dirEntry->entryType = AssetDatabaseEntry::Type::Directory;
					dirEntry->virtualRelativePath = relPath.GetFilename();
					dirEntry->parent = parentEntry;
				}
				else
				{
					auto copyPath = path;
					if (copyPath.GetExtension() == ".casset")
						return;
					if (copyPath.GetExtension() != Asset::GetAssetExtensionFor(BuiltinAssetType::SceneAsset) &&
						!copyPath.ReplaceExtension(".casset").Exists())
						return;

					auto relPathWithFixedExtension = relPath;
					if (copyPath.GetExtension() != Asset::GetAssetExtensionFor(BuiltinAssetType::SceneAsset))
					{
						relPathWithFixedExtension = relPath.ReplaceExtension(".casset");
					}

					CE::AssetDatabaseEntry* parentEntry = gameAssetsRoot;

					// Find the parent entry to add this asset to
					if (numComponents > 1)
					{
						for (auto it = relPath.begin(); it != relPath.end(); ++it)
						{
							auto str = String(it->string());
							for (auto childEntry : parentEntry->children)
							{
								if (childEntry->virtualRelativePath.GetFilename() == str)
								{
									parentEntry = childEntry;
									break;
								}
							}
						}
					}

					parentEntry->children.Add(new AssetDatabaseEntry);
					auto assetEntry = parentEntry->children.GetLast();

					assetEntry->name = path.GetFilename().RemoveExtension().GetString();
					assetEntry->extension = copyPath.GetExtension().GetString();
					assetEntry->category = AssetDatabaseEntry::Category::GameAssets;
					assetEntry->entryType = AssetDatabaseEntry::Type::Asset;
					assetEntry->virtualRelativePath = relPathWithFixedExtension.GetFilename();
					assetEntry->parent = parentEntry;

					auto builtinAssetType = Asset::GetBuiltinAssetTypeFor(relPathWithFixedExtension.GetExtension().GetString());
					assetEntry->builtinAssetType = builtinAssetType;
					assetEntry->assetClassId = 0;

					Object obj{ "Temp" };
					SerializedObject so{ Object::Type(), &obj };
					if (so.Deserialize(projectSettings.editorProjectDirectory / assetEntry->GetVirtualPath()))
					{
						assetEntry->uuid = obj.GetUuid();
						AssetDatabase::Get().uuidToAssetEntryMap[obj.GetUuid()] = assetEntry;
					}

					if (builtinAssetType == BuiltinAssetType::None) // NOT a Builtin Asset
					{
						auto assetClassName = SerializedObject::DeserializeObjectName(projectSettings.editorProjectDirectory / assetEntry->GetVirtualPath());
						if (!assetClassName.IsValid())
							return;
						auto assetClass = ClassType::FindClassByName(assetClassName);
						if (assetClass == nullptr)
							return;
						assetEntry->assetClassId = assetClass->GetTypeId();
					}
				}
			});
		}

		// *********************************************
		// Load Engine Asset entries

		if (engineDir.Exists() && engineAssetsDir.Exists())
		{
			root->children.Add(new AssetDatabaseEntry);

			auto engineAssetsRoot = root->children.GetLast();
			engineAssetsRoot->name = "Engine";
			engineAssetsRoot->category = AssetDatabaseEntry::Category::EngineAssets;
			engineAssetsRoot->entryType = AssetDatabaseEntry::Type::Directory;
			engineAssetsRoot->virtualRelativePath = "Engine/Assets";
			engineAssetsRoot->parent = root;

			engineAssetsDir.RecursivelyIterateChildren([&](const IO::Path& path)
			{
				auto relPath = IO::Path::GetRelative(path, engineAssetsDir);
				auto numComponents = std::distance(relPath.begin(), relPath.end());

				if (path.IsDirectory())
				{
					auto dirEntry = new CE::AssetDatabaseEntry;
					dirEntry->name = path.GetFilename().GetString();
					dirEntry->category = AssetDatabaseEntry::Category::EngineAssets;
					dirEntry->entryType = AssetDatabaseEntry::Type::Directory;

					CE::AssetDatabaseEntry* parentEntry = engineAssetsRoot;

					if (numComponents > 1)
					{
						for (auto it = relPath.begin(); it != relPath.end(); ++it)
						{
							auto str = String(it->string());
							for (auto& childEntry : parentEntry->children)
							{
								if (childEntry->virtualRelativePath.GetFilename() == str)
								{
									parentEntry = childEntry;
									break;
								}
							}
						}
					}

					dirEntry->virtualRelativePath = relPath.GetFilename();
					dirEntry->parent = parentEntry;
					parentEntry->children.Add(dirEntry);
				}
				else
				{
					auto copyPath = path;
					if (copyPath.GetExtension() == ".casset")
						return;
					if (copyPath.GetExtension() != ".cscene" && 
						!copyPath.ReplaceExtension(".casset").Exists())
						return;

					auto relPathWithFixedExtension = relPath;
					if (copyPath.GetExtension() != ".cscene")
					{
						relPathWithFixedExtension = relPath.ReplaceExtension(".casset");
					}

					auto assetEntry = new CE::AssetDatabaseEntry{};
					assetEntry->name = path.GetFilename().RemoveExtension().GetString();
					assetEntry->extension = copyPath.GetExtension().GetString();
					assetEntry->category = AssetDatabaseEntry::Category::EngineAssets;
					assetEntry->entryType = AssetDatabaseEntry::Type::Asset;

					CE::AssetDatabaseEntry* parentEntry = engineAssetsRoot;

					if (numComponents > 1)
					{
						for (auto it = relPath.begin(); it != relPath.end(); ++it)
						{
							auto str = String(it->string());
							for (auto& childEntry : parentEntry->children)
							{
								if (childEntry->virtualRelativePath.GetFilename() == str)
								{
									parentEntry = childEntry;
									break;
								}
							}
						}
					}

					assetEntry->virtualRelativePath = relPathWithFixedExtension.GetFilename();
					assetEntry->parent = parentEntry;
					parentEntry->children.Add(assetEntry);

					auto builtinAssetType = Asset::GetBuiltinAssetTypeFor(relPathWithFixedExtension.GetExtension().GetString());
					assetEntry->builtinAssetType = builtinAssetType;
					assetEntry->assetClassId = 0;

					Object obj{ "Temp" };
					SerializedObject so{ Object::Type(), &obj };
					if (so.Deserialize(engineDir.GetParentPath() / assetEntry->GetVirtualPath()) && obj.GetUuid() != 0)
					{
						assetEntry->uuid = obj.GetUuid();
						AssetDatabase::Get().uuidToAssetEntryMap[obj.GetUuid()] = assetEntry;
					}

					if (builtinAssetType == BuiltinAssetType::None) // NOT a Builtin Asset
					{
						auto assetClassName = SerializedObject::DeserializeObjectName(engineDir.GetParentPath() / assetEntry->GetVirtualPath());
						if (!assetClassName.IsValid())
							return;
						auto assetClass = ClassType::FindClassByName(assetClassName);
						if (assetClass == nullptr)
							return;
						assetEntry->assetClassId = assetClass->GetTypeId();
					}
				}
			});
		}

		auto gameEntry = AssetDatabase::Get().rootEntry->children[0];
		AssetDatabase::Get().assetsLoaded = true;

		//if (watcher == nullptr)
		//{
		//	watcher = new IO::FileWatcher();
		//	gameAssetsWatch = watcher->AddWatcher(gameDir, this, true);
		//	watcher->Watch();
		//}
	}

	void AssetManager::MoveAssetEntryToDirectory(AssetDatabaseEntry* entry, const IO::Path& directoryRelativePath)
	{
		auto targetDirEntry = AssetDatabase::Get().GetEntry(directoryRelativePath);
		if (targetDirEntry == nullptr)
			return;
		targetDirEntry->children.Add(entry);
		entry->parent->children.Remove(entry);
		entry->parent = targetDirEntry;
	}

	void AssetManager::DeleteAssetDatabaseEntry(AssetDatabaseEntry* entry)
	{
		if (entry == nullptr)
			return;
		if (entry->parent != nullptr)
			entry->parent->children.Remove(entry);
		delete entry;
	}

	bool AssetManager::ImportExternalAssets(Array<IO::Path> externalPaths, AssetDatabaseEntry* directoryEntry)
	{
		if (directoryEntry == nullptr)
			return false;
		if (externalPaths.IsEmpty())
			return false;

		auto basePath = ProjectSettings::Get().GetEditorProjectDirectory();
		if (directoryEntry->IsEngineContentEntry())
			basePath = PlatformDirectories::GetEngineDir().GetParentPath();
		else if (directoryEntry->IsEditorContentEntry())
			basePath = PlatformDirectories::GetEditorDir().GetParentPath();

		auto dirPath = basePath / directoryEntry->GetVirtualPath();

		for (const auto& path : externalPaths)
		{
			IO::Path::Copy(path, dirPath / path.GetFilename());
		}

		return true;
	}

	void AssetManager::HandleFileAction(IO::WatchID watchId, IO::Path directory, String fileName, IO::FileAction fileAction, String oldFileName)
	{
		QMetaObject::invokeMethod(qApp, [this]
		{
			fire OnRequestDeselectAssets();
		});

		const auto& projectSettings = ProjectSettings::Get();
        auto gameDir = projectSettings.editorProjectDirectory / "Game";

        auto relative = IO::Path::GetRelative(directory, gameDir);
        if (relative.begin() == relative.end())
            return;
        auto it = relative.begin();
        if (it->string() != "Assets" && it->string() != "Shaders")
            return;

		auto projectDir = projectSettings.editorProjectDirectory;
		auto relPath = IO::Path::GetRelative(directory, projectDir);
		auto entry = (AssetDatabaseEntry*)AssetDatabase::Get().GetEntry(relPath);
        auto fileEntry = (AssetDatabaseEntry*)AssetDatabase::Get().GetEntry(relPath / fileName);
		auto newFilePath = directory / fileName;
        auto newFilePathExtension = newFilePath.GetExtension();

		if (fileAction == IO::FileAction::Add && newFilePath.Exists())
		{
            ClassType* assetClass = nullptr;

			if (newFilePath.IsDirectory()) // Directory
			{
				auto dirEntry = new AssetDatabaseEntry();
				dirEntry->name = fileName;
				dirEntry->category = AssetDatabaseEntry::Category::GameAssets;
				dirEntry->entryType = AssetDatabaseEntry::Type::Directory;
				dirEntry->parent = entry;
				dirEntry->virtualRelativePath = fileName;
				entry->children.Add(dirEntry);
			}
            else if (newFilePath.GetExtension() == ".casset") // Product Asset
            {
                String ext = ".casset";
                bool foundMultiple = false;

                newFilePath.GetParentPath().IterateChildren([&ext, &foundMultiple, newFilePath](const IO::Path& p)
                {
                    if (!p.IsDirectory() &&
                        p.GetFilename().RemoveExtension() == newFilePath.GetFilename().RemoveExtension() &&
                        p.GetExtension() != ".casset")
                    {
                        if (ext == ".casset")
                        {
                            ext = p.GetExtension().GetString();
                        }
                        else
                        {
                            foundMultiple = true;
                        }
                    }
                });

                if (foundMultiple)
                {
                    CE_LOG(Error, All, "Found multiple assets with name {} at path {}", newFilePath.GetFilename().RemoveExtension(), newFilePath.GetParentPath());
                }

                auto assetEntry = new AssetDatabaseEntry();
                assetEntry->name = IO::Path(fileName).RemoveExtension().GetString();
                assetEntry->category = AssetDatabaseEntry::Category::GameAssets;
                assetEntry->entryType = AssetDatabaseEntry::Type::Asset;
                assetEntry->parent = entry;
                assetEntry->virtualRelativePath = fileName;
                assetEntry->extension = ext;
                entry->children.Add(assetEntry);

                QMetaObject::invokeMethod(qApp, [this, newFilePath]
                {
					fire OnAssetUpdated(newFilePath);
                });
            }
            else if (Asset::GetBuiltinAssetTypeFor(newFilePathExtension.GetString()) != BuiltinAssetType::None) // Builtin Asset
            {
                auto assetEntry = new AssetDatabaseEntry();
                assetEntry->name = IO::Path(fileName).RemoveExtension().GetString();
                assetEntry->category = AssetDatabaseEntry::Category::GameAssets;
                assetEntry->entryType = AssetDatabaseEntry::Type::Asset;
                assetEntry->parent = entry;
                assetEntry->virtualRelativePath = fileName;
                assetEntry->extension = assetEntry->virtualRelativePath.GetExtension().GetString();
                entry->children.Add(assetEntry);

                QMetaObject::invokeMethod(qApp, [this, newFilePath]
                {
					fire OnAssetUpdated(newFilePath);
                });
            }
		}
        else if (fileAction == IO::FileAction::Delete)
        {
            if (fileEntry != nullptr && fileEntry->parent != nullptr)
            {
                fileEntry->parent->children.Remove(fileEntry);
                delete fileEntry;
            }

            QMetaObject::invokeMethod(qApp, [this, newFilePath]
            {
				fire OnAssetUpdated(newFilePath);
            });
        }
		else if (fileAction == IO::FileAction::Moved)
		{
			auto oldEntry = (AssetDatabaseEntry*)AssetDatabase::Get().GetEntry(relPath / oldFileName);
			if (oldEntry != nullptr)
			{
				oldEntry->name = IO::Path(fileName).RemoveExtension().GetString();
				oldEntry->virtualRelativePath = fileName;
			}

            QMetaObject::invokeMethod(qApp, [this, newFilePath]
            {
				fire OnAssetUpdated(newFilePath);
            });
		}
        else if (fileAction == IO::FileAction::Modified)
        {
            QMetaObject::invokeMethod(qApp, [this, newFilePath]
            {
                fire OnAssetUpdated(newFilePath);
            });
        }

        QMetaObject::invokeMethod(qApp, [this]
        {
            fire OnAssetDatabaseUpdated();
        });
	}

} // namespace CE::Editor
