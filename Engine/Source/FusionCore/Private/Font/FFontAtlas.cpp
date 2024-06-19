#include "FusionCore.h"

#include "stb_image_write.h"

#undef DLL_EXPORT
#define FT_PUBLIC_FUNCTION_ATTRIBUTE

#include <ft2build.h>
#include "freetype/freetype.h"

namespace CE
{
    static constexpr SIZE_T FontAtlasSize = 512;

    FFontAtlas::FFontAtlas()
    {
        
    }

    void FFontAtlas::Init(const Array<u32>& charSet, u32 defaultFontSize)
    {
        if (atlasImageMips.NonEmpty())
            return;

        //u8* mip0 = new u8[FontAtlasSize * FontAtlasSize]; // A 2k texture atlas
        Ptr<AtlasImage> atlas = new AtlasImage;
        atlas->ptr = new u8[FontAtlasSize * FontAtlasSize];
        memset(atlas->ptr, 0, FontAtlasSize * FontAtlasSize);
        atlas->atlasSize = FontAtlasSize;
        atlas->skyline.push_back({ 0, 0, FontAtlasSize });

        atlasImageMips.Add(atlas);

        AddGlyphs(charSet, defaultFontSize);

        CMImage image = CMImage::LoadRawImageFromMemory(atlas->ptr, FontAtlasSize, FontAtlasSize, CMImageFormat::R8, CMImageSourceFormat::None, 8, 8);

        image.EncodeToPNG(PlatformDirectories::GetLaunchDir() / "Temp/FontAtlas.png");
    }

    void FFontAtlas::OnBeforeDestroy()
    {
	    Super::OnBeforeDestroy();

        delete atlasTexture; atlasTexture = nullptr;

        atlasImageMips.Clear();
    }

    void FFontAtlas::AddGlyphs(const Array<u32>& charSet, u32 fontSize)
    {
        if (atlasTexture != nullptr)
        {
            delete atlasTexture; atlasTexture = nullptr;
        }

        FT_Set_Pixel_Sizes(face, 0, fontSize);

        f32 metricHeight = face->size->metrics.height >> 6;
        f32 metricAdvance = face->size->metrics.max_advance >> 6;

        Ptr<AtlasImage> atlasMip = atlasImageMips[currentMip];

        for (int i = 0; i < charSet.GetSize(); ++i)
        {
            FT_ULong charCode = charSet[i];
            FT_Load_Char(face, charCode, FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT | FT_LOAD_TARGET_LIGHT);
            FT_Bitmap* bmp = &face->glyph->bitmap;

            FFontGlyphInfo glyph{};

            glyph.unicode = charCode;

            int width = bmp->width;
            int height = bmp->rows;

            int xOffset = face->glyph->bitmap_left;
            int yOffset = face->glyph->bitmap_top;
            int advance = face->glyph->advance.x >> 6;

            int posX, posY;
            int atlasSize = atlasMip->atlasSize;

            if (atlasMip->FindInsertionPoint(Vec2i(width, height), posX, posY))
            {
                glyph.x0 = posX;
                glyph.y0 = posY;
                glyph.x1 = posX + width;
                glyph.y1 = posY + height;

                glyph.xOffset = xOffset;
                glyph.yOffset = yOffset;
                glyph.advance = advance;

                for (int row = 0; row < bmp->rows; ++row) 
                {
                    for (int col = 0; col < bmp->width; ++col) 
                    {
                        int x = posX + col;
                        int y = posY + row;
                        atlasMip->ptr[y * atlasSize + x] = bmp->buffer[row * bmp->pitch + col];
                    }
                }

                atlasMip->glyphsByFontSize[fontSize][charCode] = glyph;
                mipIndicesByCharacter[charCode] = currentMip;

                atlasMip->UpdateSkyline(posX, posY, width, height);
            }

        }
    }

    bool FFontAtlas::AtlasImage::FindInsertionPoint(Vec2i glyphSize, int& outX, int& outY)
    {
        int bestX = -1, bestY = -1;
        int bestHeight = atlasSize, bestWidth = atlasSize;

        for (size_t i = 0; i < skyline.size(); ++i) {
            int x = skyline[i].x;
            int y = skyline[i].y;

            // Check if the glyph fits at this position
            if (x + glyphSize.width <= atlasSize && y + glyphSize.height <= atlasSize) {
                // Check if the placement improves the packing
                if (y + glyphSize.height < bestHeight || (y + glyphSize.height == bestHeight && x < bestWidth)) {
                    bestX = x;
                    bestY = y;
                    bestWidth = x + glyphSize.width;
                    bestHeight = y + glyphSize.height;
                }
            }
        }

        if (bestX == -1 || bestY == -1) 
        {
            return false;
        }

        outX = bestX;
        outY = bestY;

        return true;
    }

    void FFontAtlas::AtlasImage::UpdateSkyline(int x, int y, int width, int height)
    {
        return;

        skyline.push_back({ x, y + height, width });

        // Sort the skyline by x coordinate
        std::sort(skyline.begin(), skyline.end(), [](const SkylineSegment& a, const SkylineSegment& b) {
            return a.x < b.x;
            });

        // Merge skyline segments
        for (size_t i = 0; i < skyline.size() - 1; ++i) {
            if (skyline[i].x + skyline[i].width >= skyline[i + 1].x) {
                skyline[i].width = std::max(skyline[i].x + skyline[i].width, skyline[i + 1].x + skyline[i + 1].width) - skyline[i].x;
                skyline.erase(skyline.begin() + i + 1);
                --i;
            }
        }
    }

} // namespace CE
