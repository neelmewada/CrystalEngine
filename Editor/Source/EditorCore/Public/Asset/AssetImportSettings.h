#pragma once

#include "CoreMinimal.h"

namespace CE::Editor
{

    struct EDITORCORE_API AssetImportSettings
    {
        CE_STRUCT(AssetImportSettings)
    public:
        AssetImportSettings() = default;
        virtual ~AssetImportSettings() = default;

        static void RegisterImportSettings(StructType* importSettingsStruct, String assetExtensions);

    private:
        static HashMap<Name, StructType*> extensionToImportSettingsMap;
    };
    
} // namespace CE::Editor

CE_RTTI_STRUCT(EDITORCORE_API, CE::Editor, AssetImportSettings,
    CE_SUPER(),
    CE_ATTRIBS(),
    CE_FIELD_LIST(
        
    )
)

#define CE_REGISTER_IMPORT_SETTINGS(ImportSettingsStruct, ...)\
CE::Editor::AssetImportSettings::RegisterImportSettings(ImportSettingsStruct::Type(), #__VA_ARGS__)
