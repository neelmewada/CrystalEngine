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
    };
    
} // namespace CE


#include "TextureAsset.rtti.h"
