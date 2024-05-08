#pragma once

namespace CE::RPI
{
    class Texture;

    CLASS()
    class CORERPI_API TextureAsset : public Object
    {
        CE_CLASS(TextureAsset, Object)
    public:

        virtual ~TextureAsset();

        RPI::Texture* GetRpiTexture();

        inline u32 GetWidth() const { return width; }
        inline u32 GetHeight() const { return height; }

    private:

        RPI::Texture* rpiTexture = nullptr;

        FIELD()
        BinaryBlob source{};

        FIELD()
		RHI::SamplerAddressMode addressModeU = RHI::SamplerAddressMode::Repeat;

		FIELD()
		RHI::SamplerAddressMode addressModeV = RHI::SamplerAddressMode::Repeat;

        FIELD()
		RHI::SamplerAddressMode addressModeW = RHI::SamplerAddressMode::Repeat;

        FIELD()
        RHI::FilterMode filterMode = RHI::FilterMode::Linear;

        FIELD()
        RHI::SamplerBorderColor borderColor = RHI::SamplerBorderColor::FloatOpaqueBlack;

        FIELD()
        u8 anisotropy = 0;

        FIELD()
        u32 width = 0;

        FIELD()
        u32 height = 0;

        FIELD()
        u32 depth = 1;

        FIELD()
        u32 mipLevels = 1;

        FIELD()
        u32 arrayLayers = 1;

        FIELD()
        RHI::Format format = RHI::Format::Undefined;

        FIELD()
        RHI::Dimension dimension = RHI::Dimension::Dim2D;

#if PAL_TRAIT_BUILD_EDITOR
        friend class CE::Editor::FontAssetImportJob;
#endif

        friend class FontAtlasAsset;
    };

} // namespace CE::RPI

#include "TextureAsset.rtti.h"