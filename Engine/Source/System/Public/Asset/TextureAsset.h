#pragma once

#include "Asset.h"

namespace CE
{

    ENUM()
    enum class TextureDataType
    {
        ECONST(Display = "Color sRGB")
        ColorSRGB,
        Data,
        Grayscale,
        NormalMap,
    };

    ENUM()
    enum class TextureWrapMode
    {
        Clamped = 0,
        Repeat,
    };

    ENUM()
    enum class TextureFileFormat
    {
	    Undefined = 0,
        PNG,
        JPG,
        TIFF
    };

    CLASS()
    class SYSTEM_API TextureAsset : public CE::Asset
    {
        CE_CLASS(TextureAsset, CE::Asset)
    public:
        TextureAsset();
        virtual ~TextureAsset();

        ResourceObject* InstantiateResource() override;

        TextureFileFormat GetFileFormat();

    protected:
        FIELD(ImportSetting)
        TextureDataType textureType{};

        FIELD(ImportSetting)
        TextureWrapMode wrapMode{};
    };
    
} // namespace CE


#include "TextureAsset.rtti.h"
