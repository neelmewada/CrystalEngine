
#include "System.h"

namespace CE
{
	bool AssetDatabaseEntry::IsRoot() const
	{
		return virtualRelativePath.IsEmpty() && children.GetSize() == 0 && parent == nullptr;
	}

	IO::Path AssetDatabaseEntry::GetVirtualPath() const
	{
		IO::Path path = virtualRelativePath;
		auto parent = this->parent;

		while (parent != nullptr)
		{
			path = parent->virtualRelativePath / path;

			parent = parent->parent;
		}

		return path;
	}

	bool AssetDatabaseEntry::IsEditorContentEntry() const
	{
		return category == Category::EditorAssets || category == Category::EditorShaders;
	}

	bool AssetDatabaseEntry::IsEngineContentEntry() const
	{
		return category == Category::EngineAssets || category == Category::EngineShaders;
	}

	bool AssetDatabaseEntry::IsGameContentEntry() const
	{
		return category == Category::GameAssets || category == Category::GameShaders;
	}

	bool AssetDatabaseEntry::IsPluginContentEntry() const
	{
		return category == Category::PluginAssets || category == Category::PluginShaders;
	}

	void AssetDatabase::Initialize()
	{
		UnloadDatabase();

		rootEntry = new AssetDatabaseEntry;
		rootEntry->name = "Root";
	}

	void AssetDatabase::Shutdown()
	{
		UnloadDatabase();
	}
	

	void AssetDatabase::LoadDatabase()
	{
		if (assetsLoaded)
			return;

#if PAL_TRAIT_BUILD_EDITOR
		
#else
		LoadRuntimeAssetDatabase();
#endif
	}

	void AssetDatabase::ClearDatabase()
	{
		if (rootEntry != nullptr)
			rootEntry->ClearChildren();
	}

	void AssetDatabase::UnloadDatabase()
	{
		assetsLoaded = false;

		delete rootEntry;
		rootEntry = nullptr;
	}

	ENGINE_CONST AssetDatabaseEntry* AssetDatabase::GetEntry(IO::Path virtualPath)
	{
		return SearchForEntry(rootEntry, virtualPath);
	}

	ENGINE_CONST AssetDatabaseEntry* AssetDatabase::GetEntry(UUID assetUuid)
	{
		if (!uuidToAssetEntryMap.KeyExists(assetUuid))
			return nullptr;
		return uuidToAssetEntryMap[assetUuid];
	}

	Asset* AssetDatabase::LoadAssetAt(IO::Path virtualPath)
	{
#if PAL_TRAIT_BUILD_STANDALONE
		return LoadRuntimeAssetAt(virtualPath);
#else

		auto entry = GetEntry(virtualPath);
		if (entry == nullptr)
			return nullptr;

		auto basePath = ProjectSettings::Get().GetEditorProjectDirectory();

		if (entry->category == AssetDatabaseEntry::Category::EngineAssets ||
			entry->category == AssetDatabaseEntry::Category::EngineShaders)
		{
			basePath = PlatformDirectories::GetEngineDir().GetParentPath();
		}
		else if (entry->category == AssetDatabaseEntry::Category::GameAssets ||
			entry->category == AssetDatabaseEntry::Category::GameShaders)
		{
			
		}
		else
		{
			return nullptr;
		}

		auto fullPath = basePath / virtualPath;

		if (!fullPath.Exists())
			return nullptr;

		auto className = SerializedObject::DeserializeObjectName(fullPath);
		if (!className.IsValid())
			return nullptr;

		auto assetClass = ClassType::FindClassByName(className);
		if (assetClass == nullptr || !assetClass->IsAssignableTo(TYPEID(Asset)) || !assetClass->CanBeInstantiated())
			return nullptr;

		auto asset = (Asset*)assetClass->CreateDefaultInstance();
		asset->databaseEntry = entry;
		
		return asset;
#endif
	}

	Asset* AssetDatabase::LoadRuntimeAssetAt(IO::Path virtualPath)
	{
		// TODO: Runtime asset packaging system
		auto entry = GetEntry(virtualPath);
		if (entry == nullptr)
			return nullptr;

		auto assetsDir = PlatformDirectories::GetGameDir() / "Assets" / (String("shared") + entry->assetPackIndex + ".cpf");

		return nullptr;
	}

	AssetDatabaseEntry* AssetDatabase::SearchForEntry(AssetDatabaseEntry* searchRoot, IO::Path subVirtualPath)
	{
		if (searchRoot == nullptr || subVirtualPath.IsEmpty())
			return searchRoot;

		for (int i = 0; i < searchRoot->children.GetSize(); i++)
		{
			auto childPath = searchRoot->children[i]->virtualRelativePath;
			auto searchPath = subVirtualPath;

			auto childIt = childPath.begin();
			auto searchIt = searchPath.begin();

			for (; childIt != childPath.end() && searchIt != searchPath.end(); ++childIt, ++searchIt)
			{
				if (*childIt != *searchIt)
				{
					break;
				}
			}

			if (childIt == childPath.end()) // subVirtualPath is a SubPath of childPath
			{
				if (searchIt == searchPath.end())
					return searchRoot->children[i];
				IO::Path finalPath{};
				for (auto it = searchIt; it != searchPath.end(); ++it)
				{
					finalPath = finalPath / IO::Path((fs::path)*it);
				}
				return SearchForEntry(searchRoot->children[i], finalPath);
			}
		}

		return nullptr;
	}

	void AssetDatabase::LoadRuntimeAssetDatabase()
	{
		if (rootEntry == nullptr)
		{
			rootEntry = new AssetDatabaseEntry;
			rootEntry->name = "Root";
		}

		ClearDatabase();

		// TODO: Runtime asset packaging system

		assetsLoaded = true;
	}

} // namespace CE
