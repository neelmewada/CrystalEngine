#include "Core.h"

namespace CE
{
	AssetDatabase::AssetDatabase()
	{

	}

	AssetDatabase::~AssetDatabase()
	{
		delete rootEntry;
		rootEntry = nullptr;
	}

	void AssetDatabase::ReloadDatabase()
	{
		delete rootEntry;

		rootEntry = new AssetDatabaseEntry();


	}


} // namespace CE

CE_RTTI_CLASS_IMPL(CORE_API, CE, AssetDatabase)
