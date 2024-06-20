#include "FusionCore.h"

#include "stb_image_write.h"

#undef DLL_EXPORT
#define FT_PUBLIC_FUNCTION_ATTRIBUTE

#include <ft2build.h>
#include "freetype/freetype.h"

namespace CE
{
    static constexpr SIZE_T FontAtlasSize = 2048;
    static constexpr u64 GlyphBufferInitialCount = 512;
    static constexpr f32 GlyphBufferGrowRatio = 0.25f;
    static constexpr u32 DefaultFontSize = 13;

    static const Array<u32> gFontSizes = { 10, 13, 15, 18, 22, 28, 36, 64 };
    //static const Array<int> tempFontSizes = { 8, 10, 12, 14, 18, 22, 26, 30, 36, 42, 50, 64, 76, 90, 112, 128, 150, 155 };

    FFontAtlas::FFontAtlas()
    {
        
    }

    void FFontAtlas::Init(const Array<u32>& charSet)
    {
        ZoneScoped;

        if (atlasImageMips.NonEmpty())
            return;
        if (face == nullptr)
            return;

        int numFrames = RHI::FrameScheduler::Get()->GetFramesInFlight();

        glyphBuffer.Init("GlyphBuffer", GlyphBufferInitialCount, numFrames);

        f32 unitsPerEM = face->units_per_EM;
        f32 scaleFactor = (f32)1.0f / unitsPerEM; // 1.0f is used as a font size here

        metrics.ascender = face->ascender * scaleFactor;
        metrics.descender = face->descender * scaleFactor;
        metrics.lineGap = (face->height - (face->ascender - face->descender)) * scaleFactor;
        metrics.lineHeight = (face->ascender - face->descender + metrics.lineGap) * scaleFactor;

        Ptr<FAtlasImage> atlas = new FAtlasImage;
        atlas->ptr = new u8[FontAtlasSize * FontAtlasSize];
        memset(atlas->ptr, 0, FontAtlasSize * FontAtlasSize);
        atlas->atlasSize = FontAtlasSize;

        atlasImageMips.Add(atlas);

        AddGlyphs(charSet, DefaultFontSize);

        Array<CMImage> images;

        int idx = 0;
        for (FAtlasImage* atlasImageMip : atlasImageMips)
        {
            CMImage image = CMImage::LoadRawImageFromMemory(atlasImageMip->ptr, atlasImageMip->atlasSize, atlasImageMip->atlasSize,
                CMImageFormat::R8, CMImageSourceFormat::None, 8, 8);

            image.EncodeToPNG(PlatformDirectories::GetLaunchDir() / String::Format("Temp/FontAtlas{}.png", idx++));

            images.Add(image);
        }

        RHI::SamplerDescriptor fontSampler{};
        fontSampler.addressModeU = fontSampler.addressModeV = fontSampler.addressModeW = SamplerAddressMode::ClampToBorder;
        fontSampler.borderColor = SamplerBorderColor::FloatTransparentBlack;
        fontSampler.enableAnisotropy = false;
        fontSampler.samplerFilterMode = FilterMode::Cubic;

        atlasTexture = new RPI::Texture(images, fontSampler);

        RPI::Shader* fusionShader = FusionApplication::Get()->GetFusionShader();

        fontSrg = gDynamicRHI->CreateShaderResourceGroup(fusionShader->GetDefaultVariant()->GetSrgLayout(SRGType::PerMaterial));

        fontSrg->Bind("_FontAtlas", atlasTexture->GetRhiTexture());
        fontSrg->Bind("_FontAtlasSampler", atlasTexture->GetSamplerState());

        for (int i = 0; i < numFrames; ++i)
        {
            fontSrg->Bind(i, "_GlyphItems", glyphBuffer.GetBuffer(i));
        }

        fontSrg->FlushBindings();
    }

    FFontGlyphInfo FFontAtlas::FindOrAddGlyph(u32 charCode, u32 fontSize)
    {
        ZoneScoped;

        u32 fontSizeInAtlas = fontSize;

        static Array<u32> charSet{};
        charSet.Resize(1);
        charSet[0] = charCode;

        // Find the closest matching font size

        for (int i = 0; i < gFontSizes.GetSize(); ++i)
        {
	        if (gFontSizes[i] == fontSize || 
                (i == 0 && fontSize <= gFontSizes[i]) || 
                (i == gFontSizes.GetSize() - 1 && fontSize >= gFontSizes[i]))
	        {
                fontSizeInAtlas = gFontSizes[i];
                break;
	        }

            if (gFontSizes[i] < fontSize && fontSize < gFontSizes[i + 1])
            {
                int halfSize = (gFontSizes[i] + gFontSizes[i + 1]) / 2;
                if (fontSize <= halfSize)
                    fontSizeInAtlas = gFontSizes[i];
                else
                    fontSizeInAtlas = gFontSizes[i + 1];
                break;
            }
        }

        if (!mipIndicesByCharacter.KeyExists({ charCode, fontSizeInAtlas }))
        {
            AddGlyphs(charSet, fontSizeInAtlas);
        }

        int mipIndex = mipIndicesByCharacter[{ charCode, fontSizeInAtlas }];
        Ptr<FAtlasImage> atlasMip = atlasImageMips[mipIndex];

        if (!atlasMip->glyphsByFontSize[fontSizeInAtlas].KeyExists(charCode))
        {
            AddGlyphs(charSet, fontSizeInAtlas);
        }

        return atlasMip->glyphsByFontSize[fontSizeInAtlas][charCode];
    }

    void FFontAtlas::Flush(u32 imageIndex)
    {
        ZoneScoped;

        if (!flushRequiredPerImage[imageIndex])
            return;

        if (atlasUpdateRequired)
        {
            atlasUpdateRequired = false;

            delete atlasTexture; atlasTexture = nullptr;

            Array<CMImage> images;
            images.Reserve(atlasImageMips.GetSize());

            for (FAtlasImage* atlasImageMip : atlasImageMips)
            {
                CMImage image = CMImage::LoadRawImageFromMemory(atlasImageMip->ptr, atlasImageMip->atlasSize, atlasImageMip->atlasSize,
                    CMImageFormat::R8, CMImageSourceFormat::None, 8, 8);
                images.Add(image);
            }

            RHI::SamplerDescriptor fontSampler{};
            fontSampler.addressModeU = fontSampler.addressModeV = fontSampler.addressModeW = SamplerAddressMode::ClampToBorder;
            fontSampler.borderColor = SamplerBorderColor::FloatTransparentBlack;
            fontSampler.enableAnisotropy = false;
            fontSampler.samplerFilterMode = FilterMode::Cubic;

            atlasTexture = new RPI::Texture(images, fontSampler);

            fontSrg->Bind("_FontAtlas", atlasTexture->GetRhiTexture());
            fontSrg->Bind("_FontAtlasSampler", atlasTexture->GetSamplerState());
        }

        flushRequiredPerImage[imageIndex] = false;

        int numGlyphs = glyphDataList.GetCount();

        if (glyphBuffer.GetElementCount() < numGlyphs)
        {
            u64 totalCount = (u64)((f32)glyphBuffer.GetElementCount() * (1.0f + GlyphBufferGrowRatio));
            glyphBuffer.GrowToFit(Math::Max((u64)(numGlyphs)+64, totalCount));

            fontSrg->Bind(imageIndex, "_GlyphItems", glyphBuffer.GetBuffer(imageIndex));
        }

        glyphBuffer.GetBuffer(imageIndex)->UploadData(glyphDataList.GetData(), numGlyphs * sizeof(FGlyphData));

        fontSrg->FlushBindings();
    }

    void FFontAtlas::OnBeforeDestroy()
    {
	    Super::OnBeforeDestroy();

        glyphBuffer.Shutdown();

        delete atlasTexture; atlasTexture = nullptr;
        delete fontSrg; fontSrg = nullptr;

        atlasImageMips.Clear();
        glyphDataList.Free();
    }

    void FFontAtlas::AddGlyphs(const Array<u32>& charSet, u32 fontSize)
    {
        ZoneScoped;

        if (charSet.IsEmpty())
            return;

        static HashSet<FT_ULong> nonDisplayCharacters = { ' ', '\n', '\r', '\t' };

        FT_Set_Pixel_Sizes(face, 0, fontSize);

        FAtlasImage* atlasMip = atlasImageMips[currentMip];

        for (int i = 0; i < charSet.GetSize(); ++i)
        {
            FT_ULong charCode = charSet[i];

            if (atlasMip->glyphsByFontSize[fontSize].KeyExists(charCode))
            {
                continue;
            }

            FT_Error error = FT_Load_Char(face, charCode, FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT | FT_LOAD_TARGET_LIGHT);
            if (error != 0)
            {
	            continue;
            }

            FT_Bitmap* bmp = &face->glyph->bitmap;

            FFontGlyphInfo glyph{};

            glyph.charCode = charCode;

            int width = bmp->width;
            int height = bmp->rows;

            int xOffset = face->glyph->bitmap_left;
            int yOffset = face->glyph->bitmap_top;
            int advance = face->glyph->advance.x >> 6;

            int posX, posY;
            int atlasSize = atlasMip->atlasSize;

            bool foundEmptySpot = atlasMip->FindInsertionPoint(Vec2i(width + 1, height + 1), posX, posY);
            if (!foundEmptySpot)
            {
                currentMip++;
                atlasSize = FontAtlasSize / (SIZE_T)Math::Pow(2, currentMip);

                atlasMip = new FAtlasImage;
                atlasMip->ptr = new u8[atlasSize * atlasSize];
                memset(atlasMip->ptr, 0, atlasSize * atlasSize);
                atlasMip->atlasSize = (u32)atlasSize;

                atlasImageMips.Add(atlasMip);
                
                foundEmptySpot = atlasMip->FindInsertionPoint(Vec2i(width + 1, height + 1), posX, posY);
            }

            if (!foundEmptySpot)
            {
	            return;
            }

            if (foundEmptySpot)
            {
                glyph.x0 = posX;
                glyph.y0 = posY;
                glyph.x1 = posX + width;
                glyph.y1 = posY + height;

                glyph.xOffset = xOffset;
                glyph.yOffset = yOffset;
                glyph.advance = advance;
                glyph.fontSize = fontSize;
                glyph.index = glyphDataList.GetCount();

                Vec2 uvMin = Vec2((f32)glyph.x0 / (f32)atlasSize, (f32)glyph.y0 / (f32)atlasSize);
                Vec2 uvMax = Vec2((f32)glyph.x1 / (f32)atlasSize, (f32)glyph.y1 / (f32)atlasSize);

                if (!nonDisplayCharacters.Exists(charCode))
                {
                    glyphDataList.Insert({ .atlasUV = Vec4(uvMin, uvMax), .mipIndex = (u32)currentMip });

                    for (int row = 0; row < bmp->rows; ++row)
                    {
                        for (int col = 0; col < bmp->width; ++col)
                        {
                            int x = posX + col;
                            int y = posY + row;
                            atlasMip->ptr[y * atlasSize + x] = bmp->buffer[row * bmp->pitch + col];
                        }
                    }

                    atlasUpdateRequired = true;
                }
                else
                {
                    glyphDataList.Insert({ .atlasUV = Vec4(-1, -1, -1, -1), .mipIndex = (u32)currentMip });
                }

                atlasMip->glyphsByFontSize[fontSize][charCode] = glyph;
                mipIndicesByCharacter[{ charCode, fontSize }] = currentMip;


                for (int j = 0; j < flushRequiredPerImage.GetSize(); ++j)
                {
                    flushRequiredPerImage[j] = true;
                }
            }
        }
    }

    bool FFontAtlas::FAtlasImage::FindInsertionPoint(Vec2i glyphSize, int& outX, int& outY)
    {
        int bestRowIndex = -1;
        int bestRowHeight = INT_MAX;

        if (rows.IsEmpty())
        {
            rows.Add({ .x = glyphSize.width, .y = 0, .height = glyphSize.height });
            outX = 0;
            outY = 0;
            return true;
        }

        for (int i = 0; i < rows.GetSize(); ++i) 
        {
            int x = rows[i].x;
            int y = rows[i].y;

            // Check if the glyph fits at this position
            if (x + glyphSize.width <= atlasSize && y + glyphSize.height <= atlasSize)
            {
                if (rows[i].height >= glyphSize.height && rows[i].height < bestRowHeight)
                {
                    bestRowHeight = rows[i].height;
                    bestRowIndex = i;
                }
            }
        }

        if (bestRowIndex == -1)
        {
            RowSegment lastRow = rows.Top();
            if (lastRow.y + lastRow.height + glyphSize.height > atlasSize)
            {
	            return false;
            }

            rows.Add({ .x = glyphSize.width, .y = lastRow.y + lastRow.height, .height = glyphSize.height });

            outX = 0;
            outY = rows.Top().y;

	        return true;
        }

        outX = rows[bestRowIndex].x;
        outY = rows[bestRowIndex].y;

        rows[bestRowIndex].x += glyphSize.width;

        return true;
    }

} // namespace CE
