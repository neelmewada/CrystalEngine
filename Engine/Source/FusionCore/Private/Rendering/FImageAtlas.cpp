#include "FusionCore.h"

namespace CE
{

    FImageAtlas::FImageAtlas()
    {

    }

    FImageAtlas::~FImageAtlas()
    {
        for (int i = 0; i < atlasTexturesPerImage.GetSize(); ++i)
        {
            delete atlasTexturesPerImage[i];
            atlasTexturesPerImage[i] = nullptr;
        }
        atlasLayers.Clear();
    }

    void FImageAtlas::Init()
    {
        ZoneScoped;

        if (atlasLayers.NotEmpty())
            return;

        Ptr<FAtlasImage> atlas = new FAtlasImage;
        atlas->ptr = new u8[atlasSize * atlasSize * sizeof(u32)];
        memset(atlas->ptr, 0, atlasSize * atlasSize * sizeof(u32));
        atlas->atlasSize = atlasSize;
        atlas->layerIndex = 0;

        atlasLayers.Add(atlas);

        RPI::TextureDescriptor textureDescriptor{};
        textureDescriptor.texture.width = textureDescriptor.texture.height = atlasSize;
        textureDescriptor.texture.sampleCount = 1;
        textureDescriptor.texture.depth = 1;
        textureDescriptor.texture.dimension = Dimension::Dim2D;
        textureDescriptor.texture.arrayLayers = atlasLayers.GetSize();
        textureDescriptor.texture.name = "Fusion Image Atlas";
        textureDescriptor.texture.mipLevels = 1;
        textureDescriptor.texture.bindFlags = TextureBindFlags::ShaderRead;
        textureDescriptor.texture.format = Format::R8G8B8A8_UNORM;
        textureDescriptor.texture.defaultHeapType = MemoryHeapType::Upload;

        textureDescriptor.samplerDesc.addressModeU = SamplerAddressMode::ClampToBorder;
        textureDescriptor.samplerDesc.addressModeV = SamplerAddressMode::ClampToBorder;
        textureDescriptor.samplerDesc.addressModeW = SamplerAddressMode::ClampToBorder;
        textureDescriptor.samplerDesc.borderColor = SamplerBorderColor::FloatTransparentBlack;
        textureDescriptor.samplerDesc.enableAnisotropy = false;
        textureDescriptor.samplerDesc.samplerFilterMode = FilterMode::Linear;

        for (int i = 0; i < atlasTexturesPerImage.GetSize(); ++i)
        {
            atlasTexturesPerImage[i] = new RPI::Texture(textureDescriptor);
        }
    }

    void FImageAtlas::Flush(u32 imageIndex)
    {
        ZoneScoped;

        if (!flushRequiredPerImage[imageIndex])
            return;

        
    }

    bool FImageAtlas::AddImage(const Name& name, const CMImage& imageSource)
    {
        if (!name.IsValid() || !imageSource.IsValid())
        {
            return false;
        }

        bool foundEmptySlot = false;
        Vec2i textureSize = Vec2i(imageSource.GetWidth(), imageSource.GetHeight());
        int posX = -1, posY = -1;
        Ptr<FAtlasImage> foundAtlas = nullptr;

        u32 srcPixelSize = sizeof(u32);
        switch (imageSource.GetFormat())
        {
        case CMImageFormat::Undefined:
            return false;
        case CMImageFormat::R8:
            srcPixelSize = sizeof(u8);
            break;
        case CMImageFormat::RG8:
            srcPixelSize = sizeof(u8) * 2;
            break;
        case CMImageFormat::RGB8:
        case CMImageFormat::RGBA8:
            srcPixelSize = sizeof(u8) * 4;
            break;
        case CMImageFormat::R32:
            srcPixelSize = sizeof(f32);
            break;
        case CMImageFormat::RG32:
            srcPixelSize = sizeof(f32) * 2;
            break;
        case CMImageFormat::RGB32:
            srcPixelSize = sizeof(f32) * 4;
            break;
        case CMImageFormat::RGBA32:
            srcPixelSize = sizeof(f32) * 4;
            break;
        case CMImageFormat::R16:
            srcPixelSize = sizeof(u16);
            break;
        case CMImageFormat::RG16:
            srcPixelSize = sizeof(u16) * 2;
            break;
        case CMImageFormat::RGB16:
            srcPixelSize = sizeof(u16) * 4;
            break;
        case CMImageFormat::RGBA16:
            srcPixelSize = sizeof(u16) * 4;
            break;
        }

        for (int i = 0; i < atlasLayers.GetSize(); ++i)
        {
            Ptr<FAtlasImage> atlas = atlasLayers[i];

            foundEmptySlot = atlas->FindInsertionPoint(textureSize, posX, posY);

            if (foundEmptySlot)
            {
                foundAtlas = atlas;
	            break;
            }
        }

        if (!foundEmptySlot)
        {
            Ptr<FAtlasImage> atlas = new FAtlasImage;
            atlas->ptr = new u8[atlasSize * atlasSize * sizeof(u32)];
            memset(atlas->ptr, 0, atlasSize * atlasSize * sizeof(u32));
            atlas->atlasSize = atlasSize;
            atlas->layerIndex = atlasLayers.GetSize();

            atlasLayers.Add(atlas);

            foundEmptySlot = atlas->FindInsertionPoint(textureSize, posX, posY);

            if (!foundEmptySlot)
            {
                return false;
            }
            else
            {
                foundAtlas = atlas;
            }
        }

        for (int x = 0; x < textureSize.x; ++x)
        {
	        for (int y = 0; y < textureSize.y; ++y)
	        {
                Vec2i dstPos = Vec2i(posX + x, posY + y);

                u8* dstPixel = foundAtlas->ptr + (foundAtlas->atlasSize * dstPos.y + dstPos.x) * sizeof(u32);
                u8* srcPixel = (u8*)imageSource.GetDataPtr() + (textureSize.y * y + x) * srcPixelSize;
                u8* r = dstPixel;
                u8* g = dstPixel + 1;
                u8* b = dstPixel + 2;
                u8* a = dstPixel + 3;

                // TODO: 
                switch (imageSource.GetFormat())
                {
                case CMImageFormat::Undefined:
	                break;
                case CMImageFormat::R8:
                    *r = *g = *b = *srcPixel;
                    *a = (u8)255;
	                break;
                case CMImageFormat::RG8:
                    *r = *g = *b = *srcPixel;
                    *a = *(srcPixel + 1);
	                break;
                case CMImageFormat::RGB8:
                    *r = *srcPixel;
                    *g = *(srcPixel + 1);
                    *b = *(srcPixel + 2);
                    *a = (u8)255;
	                break;
                case CMImageFormat::RGBA8:
                    *r = *srcPixel;
                    *g = *(srcPixel + 1);
                    *b = *(srcPixel + 2);
                    *a = *(srcPixel + 3);
	                break;
                case CMImageFormat::R32:
                    *r = *g = *b = static_cast<u8>(*((f32*)srcPixel) * 255.0f);
                    *a = (u8)255;
	                break;
                case CMImageFormat::RG32:
                    *r = *g = *b = static_cast<u8>(*((f32*)srcPixel) * 255.0f);
                    *a = static_cast<u8>(*((f32*)srcPixel + 1) * 255.0f);
	                break;
                case CMImageFormat::RGB32:
                    *r = static_cast<u8>(*((f32*)srcPixel) * 255.0f);
                    *g = static_cast<u8>(*((f32*)srcPixel + 1) * 255.0f);
                    *b = static_cast<u8>(*((f32*)srcPixel + 2) * 255.0f);
                    *a = (u8)255;
	                break;
                case CMImageFormat::RGBA32:
                    *r = static_cast<u8>(*((f32*)srcPixel) * 255.0f);
                    *g = static_cast<u8>(*((f32*)srcPixel + 1) * 255.0f);
                    *b = static_cast<u8>(*((f32*)srcPixel + 2) * 255.0f);
                    *a = static_cast<u8>(*((f32*)srcPixel + 3) * 255.0f);
	                break;
                case CMImageFormat::R16:
	                break;
                case CMImageFormat::RG16:
	                break;
                case CMImageFormat::RGB16:
	                break;
                case CMImageFormat::RGBA16:
	                break;
                }
	        }
        }


        return false;
    }

    bool FImageAtlas::FAtlasImage::FindInsertionPoint(Vec2i textureSize, int& outX, int& outY)
    {
        ZoneScoped;

        int bestRowIndex = -1;
        int bestRowHeight = INT_MAX;

        if (rows.IsEmpty())
        {
            rows.Add({ .x = textureSize.width, .y = 0, .height = textureSize.height });
            outX = 0;
            outY = 0;
            return true;
        }

        for (int i = 0; i < rows.GetSize(); ++i)
        {
            int x = rows[i].x;
            int y = rows[i].y;

            // Check if the glyph fits at this position
            if (x + textureSize.width <= atlasSize && y + textureSize.height <= atlasSize)
            {
                if (rows[i].height >= textureSize.height && rows[i].height < bestRowHeight)
                {
                    bestRowHeight = rows[i].height;
                    bestRowIndex = i;
                }
            }
        }

        if (bestRowIndex == -1)
        {
            RowSegment lastRow = rows.Top();
            if (lastRow.y + lastRow.height + textureSize.height > atlasSize)
            {
                return false;
            }

            rows.Add({ .x = textureSize.width, .y = lastRow.y + lastRow.height, .height = textureSize.height });

            outX = 0;
            outY = rows.Top().y;

            return true;
        }

        outX = rows[bestRowIndex].x;
        outY = rows[bestRowIndex].y;

        rows[bestRowIndex].x += textureSize.width;

        return true;
    }

} // namespace CE

