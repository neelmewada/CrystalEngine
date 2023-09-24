#pragma once

namespace CE
{
#if PAL_TRAIT_BUILD_EDITOR
	namespace Editor { class EditorAssetManager; }
#endif

	struct IAssetRegistryListener
	{
		virtual void OnAssetImported(const Name& packageName, const Name& sourcePath = "") {}

		virtual void OnAssetDeleted(const Name& packageName) {}
	};

	CLASS()
	class SYSTEM_API AssetRegistry : public Object, IO::IFileWatchListener
	{
		CE_CLASS(AssetRegistry, Object)
	public:

		AssetRegistry();
		virtual ~AssetRegistry();

		void Shutdown();

		static AssetRegistry* Get();

		inline PathTree& GetCachedDirectoryPathTree()
		{
			return directoryTree;
		}

		struct SourceAssetChange
		{
			IO::FileAction fileAction{};
			IO::Path currentPath{};
			IO::Path oldPath{};
            u64 fileSize = 0;

			inline bool IsValid()
			{
				return !currentPath.IsEmpty() || !oldPath.IsEmpty();
			}
		};

		// - Asset Registry API -

		AssetData* GetPrimaryAssetByPath(const Name& path);
		Array<AssetData*> GetAssetsByPath(const Name& path);
		AssetData* GetAssetBySourcePath(const Name& sourcePath);

		Array<AssetData*> GetPrimaryAssetsInSubPath(const Name& parentPath);

		Array<String> GetSubDirectoriesAtPath(const Name& path);
		PathTreeNode* GetDirectoryNode(const Name& path);



	protected:

		/// Caches path tree structure
		void InitializeCache();

		// Inherited via IFileWatchListener
		virtual void HandleFileAction(IO::WatchID watchId, IO::Path directory, const String& fileName, IO::FileAction fileAction, const String& oldFileName) override;

		// - Internal API -

		void OnAssetImported(const Name& packageName, const Name& sourcePath = "");

		void OnAssetDeleted(const Name& packageName);

	public:

		// Events

		MultiCastDelegate<void(void)> onAssetRegistryModified{};
		MultiCastDelegate<void(const Name&)> onAssetImported{};

	private:

		void AddAssetEntry(const Name& packageName, AssetData* assetData);
		void DeleteAssetEntry(const Name& packageName);

		PathTree directoryTree{};
		PathTree cachedPathTree{};

		b8 cacheInitialized = false;
        
        Array<SourceAssetChange> sourceChanges{};

		static AssetRegistry* singleton;

		friend class AssetManager;

#if PAL_TRAIT_BUILD_EDITOR
		friend class CE::Editor::EditorAssetManager;
#endif
		IO::FileWatcher fileWatcher{};
		IO::WatchID fileWatchID = 0;
		
		Mutex mutex{};

		IAssetRegistryListener* listener = nullptr;

		// Asset Registry State

		Array<AssetData*> allAssetDatas{};

		HashMap<UUID, AssetData*> cachedPrimaryAssetByPackageUuid{};
		HashMap<Name, AssetData*> cachedPrimaryAssetByPath{};
		HashMap<Name, Array<AssetData*>> cachedAssetsByPath{};
		HashMap<Name, AssetData*> cachedAssetBySourcePath{};

		/// List of primary assets in the sub-path of a path
		HashMap<Name, Array<AssetData*>> cachedPrimaryAssetsByParentPath{};
		
	};
    
} // namespace CE

#include "AssetRegistry.rtti.h"
