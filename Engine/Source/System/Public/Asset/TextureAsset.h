#pragma once

#include "CoreMinimal.h"

#include "Asset.h"

namespace CE
{
#if PAL_TRAIT_BUILD_EDITOR
    namespace Editor
    {
        class TextureAssetImportSettings;
    }
#endif

    enum class TextureDataType
    {
        ColorSRGB,
        Color,
        Grayscale,
        NormalMap,
    };

    class SYSTEM_API TextureAsset : public Asset
    {
        CE_CLASS(TextureAsset, Asset)
    public:
        TextureAsset();
        virtual ~TextureAsset();


    protected:

#if PAL_TRAIT_BUILD_EDITOR
        friend class CE::Editor::TextureAssetImportSettings;
#endif
    };
    
} // namespace CE


// Texture Data Type

CE_RTTI_ENUM(SYSTEM_API, CE, TextureDataType,
    CE_ATTRIBS(),
    CE_CONST(ColorSRGB, Display = "Color SRGB"),
    CE_CONST(Color,     Display = "Color"),
    CE_CONST(Grayscale, Display = "Grayscale"),
    CE_CONST(NormalMap, Display = "Normal Map"),
)

// Texture Asset

CE_RTTI_CLASS(SYSTEM_API, CE, TextureAsset,
    CE_SUPER(CE::Asset),
    CE_NOT_ABSTRACT,
    CE_ATTRIBS(),
    CE_FIELD_LIST(),
    CE_FUNCTION_LIST()
)


