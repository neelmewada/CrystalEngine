#pragma once

#include "CoreMinimal.h"

namespace CE
{
    class Asset;
}

namespace CE::Editor
{

    class EDITORCORE_API AssetImportSettings
    {
        CE_CLASS(AssetImportSettings)
    public:
        AssetImportSettings() = default;
        virtual ~AssetImportSettings() = default;

        static void RegisterImportSettings(ClassType* importSettingsClass, ClassType* assetClass, String assetExtensions);

        static ClassType* GetImportSettingsClassFor(String assetExtension);
        static ClassType* GetAssetClassFor(ClassType* importSettings);

        virtual bool ApplyImportSettings(Asset* targetAsset) = 0;

    private:
        static HashMap<Name, ClassType*> extensionToImportSettingsMap;
        static HashMap<ClassType*, ClassType*> importSettingsToAssetClassTypeMap;
    };
    
} // namespace CE::Editor

CE_RTTI_CLASS(EDITORCORE_API, CE::Editor, AssetImportSettings,
    CE_SUPER(),
    CE_ABSTRACT,
    CE_ATTRIBS(),
    CE_FIELD_LIST(
        
    ),
    CE_FUNCTION_LIST()
)

#define CE_REGISTER_IMPORT_SETTINGS(ImportSettingsClass, AssetClass, ...)\
CE_REGISTER_TYPES(ImportSettingsClass)\
CE::Editor::AssetImportSettings::RegisterImportSettings(ImportSettingsClass::Type(), AssetClass::Type(), #__VA_ARGS__)
