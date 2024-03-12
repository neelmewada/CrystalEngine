#pragma once

namespace CE::RPI
{

    STRUCT()
    struct CORERPI_API FontGlyphLayout
    {
        CE_STRUCT(FontGlyphLayout)
    public:

        FIELD()
        u32 unicode = 0;

        FIELD()
        int x0 = 0;

        FIELD()
        int y0 = 0;

        FIELD()
        int x1 = 0;

        FIELD()
        int y1 = 0;

        FIELD()
        int xOffset = 0; // left bearing when rendering

        FIELD()
        int yOffset = 0;    // top bearing when rendering

        FIELD()
        int advance = 0;        // x advance when rendering

        inline int GetWidth() const { return x1 - x0; }
        inline int GetHeight() const { return y1 - y0; }
    };


} // namespace CE::RPI

#include "FontAtlasLayout.rtti.h"