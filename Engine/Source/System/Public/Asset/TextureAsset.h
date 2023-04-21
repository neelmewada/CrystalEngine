#pragma once

#include "Asset.h"

namespace CE
{

    ENUM()
    enum class TextureDataType
    {
        ECONST(Display = "Color sRGB")
        ColorSRGB,
        Color,
        Grayscale,
        NormalMap,
    };

    ENUM()
    enum class TextureWrapMode
    {
        Clamped = 0,
        Repeat,
    };

    CLASS()
    class SYSTEM_API TextureAsset : public CE::Asset
    {
        CE_CLASS(TextureAsset, CE::Asset)
    public:
        TextureAsset();
        virtual ~TextureAsset();

        ResourceObject* InstantiateResource() override;

    protected:
        FIELD(ImportSetting)
        TextureDataType textureType{};

        FIELD(ImportSetting)
        TextureWrapMode wrapMode{};
    };
    
} // namespace CE


#include "TextureAsset.rtti.h"
