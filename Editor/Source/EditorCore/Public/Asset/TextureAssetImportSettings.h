#pragma once

#include "CoreMinimal.h"

#include "AssetImportSettings.h"

namespace CE::Editor
{

    enum class TextureDataType
    {
        ColorSRGB,
        Color,
        Grayscale,
        NormalMap,
    };

    class EDITORCORE_API TextureAssetImportSettings : public AssetImportSettings
    {
        CE_CLASS(TextureAssetImportSettings, AssetImportSettings)
    public:

        virtual bool ApplyImportSettings(Asset* targetAsset) override;

    protected:
        
        TextureDataType textureType{};
    };

} // namespace CE::Editor

CE_RTTI_ENUM(EDITORCORE_API, CE::Editor, TextureDataType,
    CE_ATTRIBS(),
    CE_CONST(ColorSRGB, Display = "Color SRGB"),
    CE_CONST(Color, Display = "Color"),
    CE_CONST(Grayscale, Display = "Grayscale"),
    CE_CONST(NormalMap, Display = "Normal Map"),
)

CE_RTTI_CLASS(EDITORCORE_API, CE::Editor, TextureAssetImportSettings,
    CE_SUPER(CE::Editor::AssetImportSettings),
    CE_NOT_ABSTRACT,
    CE_ATTRIBS(),
    CE_FIELD_LIST(
        CE_FIELD(textureType, Display = "Texture Type")
    ),
    CE_FUNCTION_LIST()
)
