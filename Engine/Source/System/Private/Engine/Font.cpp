#include "System.h"

namespace CE
{

    Font::~Font()
    {

    }

    const RPI::FontGlyphLayout& Font::GetGlyphLayout(u32 unicode)
    {
        if (glyphLayoutsCache.IsEmpty())
        {
            CacheGlyphLayouts();
        }

        return glyphLayoutsCache[unicode];
    }

    void Font::OnAfterDeserialize()
    {
        Super::OnAfterDeserialize();

        CacheGlyphLayouts();
    }

    void Font::CacheGlyphLayouts()
    {
        glyphLayoutsCache.Clear();

        for (u32 i = 0; glyphLayouts.GetSize(); i++)
        {
            glyphLayoutsCache[glyphLayouts[i].unicode] = glyphLayouts[i];
        }
    }

} // namespace CE
