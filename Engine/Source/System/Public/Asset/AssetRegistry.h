#pragma once

namespace CE
{
#if PAL_TRAIT_BUILD_EDITOR
	namespace Editor { class EditorAssetManager; }
#endif


	CLASS()
	class SYSTEM_API AssetRegistry : public Object, IO::IFileWatchListener
	{
		CE_CLASS(AssetRegistry, Object)
	public:

		AssetRegistry();
		virtual ~AssetRegistry();

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



	protected:

		/// Caches path tree structure
		void CachePathTree();

		// Inherited via IFileWatchListener
		virtual void HandleFileAction(IO::WatchID watchId, IO::Path directory, const String& fileName, IO::FileAction fileAction, const String& oldFileName) override;

	private:

		PathTree directoryTree{};
		PathTree cachedPathTree{};

		b8 pathTreeCached = false;
        
        Array<SourceAssetChange> sourceChanges{};

		static AssetRegistry* singleton;

		friend class AssetManager;

#if PAL_TRAIT_BUILD_EDITOR
		friend class CE::Editor::EditorAssetManager;
#endif
		IO::FileWatcher fileWatcher{};
		IO::WatchID fileWatchID = 0;
		
		Mutex mutex{};

		// Asset Registry State

		Array<AssetData*> allAssetDatas{};

		HashMap<Name, Array<AssetData*>> cachedAssetsByPath{};
		HashMap<Name, AssetData*> cachedAssetsBySourceFilePath{};
	};
    
} // namespace CE

#include "AssetRegistry.rtti.h"
