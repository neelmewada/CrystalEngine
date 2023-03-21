
#include "Asset/AssetImportSettings.h"

namespace CE::Editor
{

	HashMap<Name, StructType*> AssetImportSettings::extensionToImportSettingsMap{};


	void AssetImportSettings::RegisterImportSettings(StructType* importSettingsStruct, String assetExtensions)
	{
		assetExtensions = assetExtensions.RemoveWhitespaces();
		Array<String> list = assetExtensions.Split(',');

		for (const auto& extension : list)
		{
			extensionToImportSettingsMap[extension] = importSettingsStruct;
		}
	}

}

CE_RTTI_STRUCT_IMPL(EDITORCORE_API, CE::Editor, AssetImportSettings)
