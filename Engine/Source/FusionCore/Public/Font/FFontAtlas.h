#pragma once

typedef struct FT_LibraryRec_* FT_Library;
typedef struct FT_FaceRec_* FT_Face;

namespace CE
{
    struct FFontMetrics
    {
        f32 ascender = 0;
        f32 descender = 0;
        f32 lineGap = 0;
        f32 lineHeight = 0;
    };

    struct FFontGlyphInfo
    {
        u32 unicode = 0;
        int x0 = 0;
        int y0 = 0;
        int x1 = 0;
        int y1 = 0;
        int xOffset = 0; // left bearing when rendering
        int yOffset = 0;    // top bearing when rendering
        int advance = 0;        // x advance when rendering

        inline int GetWidth() const { return x1 - x0; }
        inline int GetHeight() const { return y1 - y0; }
    };

    CLASS()
    class FUSIONCORE_API FFontAtlas : public Object
    {
        CE_CLASS(FFontAtlas, Object)
    public:

        FFontAtlas();

        void Init(const Array<u32>& characterSet, u32 defaultFontSize);

    private:

        void OnBeforeDestroy() override;

        void AddGlyphs(const Array<u32>& characterSet, u32 fontSize);

        RPI::Texture* fontAtlas = nullptr;

    private:

        using CharCode = u32;

        struct SkylineSegment {
            int x, y;
            int width;
        };

        struct AtlasImage : IntrusiveBase
        {
            virtual ~AtlasImage()
            {
                delete ptr; ptr = nullptr;
            }

            u8* ptr = nullptr;
            u32 atlasSize = 0;
            std::vector<SkylineSegment> skyline;

            HashMap<u32, HashMap<CharCode, FFontGlyphInfo>> glyphsByFontSize;

            void UpdateSkyline(int x, int y, int width, int height);

            bool FindInsertionPoint(Vec2i glyphSize, int& outX, int& outY);
        };

        HashMap<CharCode, int> mipIndicesByCharacter;

        Array<Ptr<AtlasImage>> atlasImageMips;
        int currentMip = 0;

        RPI::Texture* atlasTexture = nullptr;

        FT_Library ft = nullptr;
        FT_Face face = nullptr;
        FT_Face faceItalic = nullptr;

        friend class FFontManager;
    };
    
} // namespace CE

#include "FFontAtlas.rtti.h"