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
    struct CORERPI_API FontGlyphPlaneBounds
    {
        CE_STRUCT(FontGlyphPlaneBounds)
    public:



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


    };

    STRUCT()
    struct CORERPI_API FontAtlasLayout
    {
        CE_STRUCT(FontAtlasLayout)
    public:

        FIELD()
        FontAtlasDesc atlas{};


    };

} // namespace CE::RPI

#include "FontAtlas.rtti.h"