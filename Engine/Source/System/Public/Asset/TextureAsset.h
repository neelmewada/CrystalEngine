#pragma once

#include "Asset.h"

namespace CE
{
#if PAL_TRAIT_BUILD_EDITOR
    namespace Editor
    {
        class TextureAssetImportSettings;
    }
#endif

    ENUM()
    enum class TextureDataType
    {
        ECONST(Display = "Color sRGB")
        ColorSRGB,
        Color,
        Grayscale,
        NormalMap,
    };

    CLASS()
    class SYSTEM_API TextureAsset : public CE::Asset
    {
        CE_CLASS(TextureAsset, CE::Asset)
    public:
        TextureAsset();
        virtual ~TextureAsset();


    protected:
        FIELD()
        TextureDataType textureType{};

#if PAL_TRAIT_BUILD_EDITOR
        friend class CE::Editor::TextureAssetImportSettings;
#endif
    };
    
} // namespace CE


#include "TextureAsset.rtti.h"
