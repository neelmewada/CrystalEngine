
#include "System.h"

namespace CE
{

	void AssetDatabase::Initialize()
	{
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
		//LoadAssetDatabaseForEditor();
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

	const AssetDatabaseEntry* AssetDatabase::GetEntry(IO::Path virtualPath)
	{
		return SearchForEntry(rootEntry, virtualPath);
	}

	const AssetDatabaseEntry* AssetDatabase::SearchForEntry(AssetDatabaseEntry* searchRoot, IO::Path subVirtualPath)
	{
		if (searchRoot == nullptr || subVirtualPath.IsEmpty())
			return searchRoot;

		for (int i = 0; i < searchRoot->children.GetSize(); i++)
		{
			auto childPath = searchRoot->children[i].virtualRelativePath;
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
					return &searchRoot->children[i];
				IO::Path finalPath{};
				for (auto it = searchIt; it != searchPath.end(); ++it)
				{
					finalPath = finalPath / IO::Path((fs::path)*it);
				}
				return SearchForEntry(&searchRoot->children[i], finalPath);
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
