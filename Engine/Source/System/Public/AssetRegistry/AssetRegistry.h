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
			IO::FileAction fileAsction{};
			IO::Path currentPath{};
			IO::Path oldPath{};
		};

		// - Asset Registry API -

		/// Caches path tree structure
		void CachePathTree();


	protected:

		// Inherited via IFileWatchListener
		virtual void HandleFileAction(IO::WatchID watchId, IO::Path directory, const String& fileName, IO::FileAction fileAction, const String& oldFileName) override;

	private:

		PathTree directoryTree{};
		PathTree cachedPathTree{};

		b8 pathTreeCached = false;

		static AssetRegistry* singleton;

		friend class AssetManager;

#if PAL_TRAIT_BUILD_EDITOR
		friend class CE::Editor::EditorAssetManager;
#endif
		IO::FileWatcher fileWatcher{};
		IO::WatchID fileWatchID = 0;
		
		Mutex mutex{};
	};
    
} // namespace CE

#include "AssetRegistry.rtti.h"
