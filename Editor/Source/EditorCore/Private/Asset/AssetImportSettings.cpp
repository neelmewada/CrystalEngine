
#include "Asset/AssetImportSettings.h"

namespace CE::Editor
{

	HashMap<Name, ClassType*> AssetImportSettings::extensionToImportSettingsMap{};

	HashMap<ClassType*, ClassType*> AssetImportSettings::importSettingsToAssetClassTypeMap{};

	void AssetImportSettings::RegisterImportSettings(ClassType* importSettingsClass, ClassType* assetClass, String assetExtensions)
	{
		if (!importSettingsClass->CanBeInstantiated() || !assetClass->CanBeInstantiated())
			return;

		assetExtensions = assetExtensions.RemoveWhitespaces();
		Array<String> list = assetExtensions.Split(',');

		for (const auto& extension : list)
		{
			extensionToImportSettingsMap[extension] = importSettingsClass;
		}

		importSettingsToAssetClassTypeMap[importSettingsClass] = assetClass;
	}

	ClassType* AssetImportSettings::GetImportSettingsClassFor(String assetExtension)
	{
		return extensionToImportSettingsMap[assetExtension];
	}

	ClassType* AssetImportSettings::GetAssetClassFor(ClassType* importSettingsClass)
	{
		return importSettingsToAssetClassTypeMap[importSettingsClass];
	}

}

CE_RTTI_CLASS_IMPL(EDITORCORE_API, CE::Editor, AssetImportSettings)

