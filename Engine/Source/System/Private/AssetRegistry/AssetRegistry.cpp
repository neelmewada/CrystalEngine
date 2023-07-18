#include "System.h"

namespace CE
{
	AssetRegistry* AssetRegistry::singleton = nullptr;

	AssetRegistry::AssetRegistry()
	{
		if (singleton == nullptr)
			singleton = this;

		
	}

	AssetRegistry::~AssetRegistry()
	{
		if (singleton == this)
			singleton = nullptr;
		

	}

	void AssetRegistry::LoadCachedPathTree()
	{
		if (pathTreeLoaded)
			return;

		// Clear the path tree
		pathTree.RemoveAll();



		pathTreeLoaded = true;
	}

} // namespace CE
