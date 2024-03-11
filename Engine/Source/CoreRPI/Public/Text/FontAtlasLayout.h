#pragma once

namespace CE::RPI
{
    STRUCT()
    struct CORERPI_API FontAtlasDesc
    {
        CE_STRUCT(FontAtlasDesc)
    public:

        FIELD()
        Name type{};

        FIELD()
        f32 distanceRange = 0;

        FIELD()
        f32 size = 0;

        FIELD()
        u32 width = 0;

        FIELD()
        u32 height = 0;

        FIELD()
        Name yOrigin{};

    };

    STRUCT()
    struct CORERPI_API FontGlyphMetrics
    {
        CE_STRUCT(FontGlyphMetrics)
    public:

        FIELD()
        u32 emSize = 0;

        FIELD()
        f32 lineHeight = 0;

        FIELD()
        f32 ascender = 0;

        FIELD()
        f32 descender = 0;

        FIELD()
        f32 underlineY = 0;

        FIELD()
        f32 underlineThickness = 0;
    };

    STRUCT()
    struct CORERPI_API FontGlyphDesc
    {
        CE_STRUCT(FontGlyphDesc)
    public:

        FIELD()
        u32 unicode = 0;

        FIELD()
        f32 advance = 0;

        FIELD()
        Vec4 planeBounds{};

        FIELD()
        Vec4 atlasBounds{};
    };

    STRUCT()
    struct CORERPI_API FontAtlasLayout
    {
        CE_STRUCT(FontAtlasLayout)
    public:

        FIELD()
        FontAtlasDesc atlas{};

        FIELD()
        FontGlyphMetrics metrics{};

        FIELD()
        Array<FontGlyphDesc> glyphs{};

    };

} // namespace CE::RPI

#include "FontAtlasLayout.rtti.h"