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

    ENUM(FlagBits = 32)
    enum class TextureDataType
    {
        ColorSRGB,
        Color,
        Grayscale,
        NormalMap,
    };

    CLASS(MaxSize = 32, EditAnywhere, Display = "Test Array", meta = (SomeValue = 31, OtherValue = "Some Text"))
    class SYSTEM_API TextureAsset : public CE::Asset
    {
        CE_CLASS(TextureAsset, CE::Asset)
    public:
        TextureAsset();
        virtual ~TextureAsset();

        FUNCTION()
        void TestFunction(s32 value);

        FIELD(Display = "Test String")
        String testString{};

        FIELD(MaxSize = 32, EditAnywhere, Display = "Test Array")
        Array<String> testArray;

    protected:
        
        TextureDataType textureType{};

#if PAL_TRAIT_BUILD_EDITOR
        friend class CE::Editor::TextureAssetImportSettings;
#endif
    };
    
} // namespace CE

//#include "TextureAsset.rtti.h"

// Texture Data Type

CE_RTTI_ENUM(SYSTEM_API, CE, TextureDataType,
    CE_ATTRIBS(),
    CE_CONST(ColorSRGB, Display = "Color SRGB"),
    CE_CONST(Color,     Display = "Color"),
    CE_CONST(Grayscale, Display = "Grayscale"),
    CE_CONST(NormalMap, Display = "Normal Map"),
)

// Texture Asset

//CE_RTTI_CLASS(SYSTEM_API, CE, TextureAsset,
//    CE_SUPER(CE::Asset),
//    CE_NOT_ABSTRACT,
//    CE_ATTRIBS(),
//    CE_FIELD_LIST(
//        CE_FIELD(textureType)
//    ),
//    CE_FUNCTION_LIST()
//)


