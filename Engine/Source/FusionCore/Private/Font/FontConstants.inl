#pragma once

namespace CE
{
    static constexpr SIZE_T FontAtlasSize = 2048;
    static constexpr u64 GlyphBufferInitialCount = 512;
    static constexpr f32 GlyphBufferGrowRatio = 0.25f;
    static constexpr u32 DefaultFontSize = 13;

    static const Array<u32> gFontSizes = { 10, 15, 18, 22, 28, 36, 64 };
}
