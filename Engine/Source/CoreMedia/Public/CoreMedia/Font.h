#pragma once

namespace CE
{
    struct CMGlyphInfo 
    {
        u32 charCode = 0;
        int x0, y0, x1, y1;	// coords of glyph in the texture atlas
        int xOffset, yOffset;   // left & top bearing when rendering
        int advance;        // x advance when rendering
        int width; int height;
    };

    struct CharRange
    {
        CharRange() = default;
        CharRange(u32 c) { charCode = c; this->range = Vec2i(0, 0); }
        CharRange(s32 min, s32 max) { this->range = Vec2i(min, max); charCode = 0; }

        u32 charCode = 0;
        Vec2i range{};
    };

    struct CMFontAtlasGenerateInfo
    {
        Array<CharRange> charSetRanges{};
        u32 padding = 0;
        u32 fontSize = 16;
    };
    
    class COREMEDIA_API CMFontAtlas final
    {
    public:

        ~CMFontAtlas();

        static CMFontAtlas* GenerateFromFontFile(const IO::Path& filePath, const CMFontAtlasGenerateInfo& generateInfo);

        inline const CMImage& GetAtlas() const { return atlas; }

        inline const Array<CMGlyphInfo>& GetGlyphInfos() const { return glyphInfos; }

    private:

        CMImage atlas{};

        Array<CMGlyphInfo> glyphInfos{};
    };

} // namespace CE
