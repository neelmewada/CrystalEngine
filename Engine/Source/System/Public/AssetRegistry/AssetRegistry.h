#pragma once

namespace CE
{
#if PAL_TRAIT_BUILD_EDITOR
	namespace Editor { class EditorAssetManager; }
#endif

	CLASS()
	class SYSTEM_API AssetRegistry : public Object
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

		// - Asset Registry API -

		/// Caches path tree structure
		void CachePathTree();



	protected:


	private:

		PathTree directoryTree{};
		PathTree cachedPathTree{};

		b8 pathTreeCached = false;

		static AssetRegistry* singleton;

		friend class AssetManager;

#if PAL_TRAIT_BUILD_EDITOR
		friend class CE::Editor::EditorAssetManager;
#endif
	};
    
} // namespace CE

#include "AssetRegistry.rtti.h"
