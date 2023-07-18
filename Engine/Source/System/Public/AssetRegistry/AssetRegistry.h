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

		inline PathTree& GetCachedPathTree()
		{
			return pathTree;
		}

		void LoadCachedPathTree();

	private:

		PathTree pathTree{};

		b8 pathTreeLoaded = false;

		static AssetRegistry* singleton;
	};
    
} // namespace CE

#include "AssetRegistry.rtti.h"
