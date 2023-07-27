#pragma once

namespace CE
{
	CLASS()
	class SYSTEM_API AssetRegistry : public Object
	{
		CE_CLASS(AssetRegistry, Object)
	public:

		AssetRegistry();
		virtual ~AssetRegistry();

		inline PathTree& GetCachedDirectoryPathTree()
		{
			return directoryTree;
		}

		inline PathTree& GetCachedPathTree()
		{
			return pathTree;
		}

		void CachePathTree();

	private:

		PathTree directoryTree{};
		PathTree pathTree{};

		b8 pathTreeCached = false;

		static AssetRegistry* singleton;
	};
    
} // namespace CE

#include "AssetRegistry.rtti.h"
