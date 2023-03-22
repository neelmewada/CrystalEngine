#pragma once

#include "AssetImportSettings.h"

namespace CE::Editor
{

    class EDITORCORE_API TextureAssetImportSettings : public AssetImportSettings
    {
        CE_CLASS(TextureAssetImportSettings, AssetImportSettings)
    public:

        virtual bool ApplyImportSettings(Asset* targetAsset) override;

    protected:
        Vec3i resolution{};
    };

} // namespace CE::Editor

CE_RTTI_CLASS(EDITORCORE_API, CE::Editor, TextureAssetImportSettings,
    CE_SUPER(CE::Editor::AssetImportSettings),
    CE_NOT_ABSTRACT,
    CE_ATTRIBS(),
    CE_FIELD_LIST(
        CE_FIELD(resolution)
    ),
    CE_FUNCTION_LIST()
)
