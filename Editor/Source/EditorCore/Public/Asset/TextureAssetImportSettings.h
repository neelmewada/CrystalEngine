#pragma once

#include "AssetImportSettings.h"

namespace CE::Editor
{

    struct EDITORCORE_API TextureAssetImportSettings : public AssetImportSettings
    {
        CE_STRUCT(TextureAssetImportSettings, AssetImportSettings)
    public:
        
        Vec3i resolution{};
    };

} // namespace CE::Editor

CE_RTTI_STRUCT(EDITORCORE_API, CE::Editor, TextureAssetImportSettings,
    CE_SUPER(CE::Editor::AssetImportSettings),
    CE_ATTRIBS(),
    CE_FIELD_LIST(
        CE_FIELD(resolution)
    )
)
