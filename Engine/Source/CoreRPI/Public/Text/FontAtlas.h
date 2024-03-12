#pragma once

namespace CE::RPI
{

    CLASS()
    class CORERPI_API FontAtlas : public Object
    {
        CE_CLASS(FontAtlas, Object)
    public:

        virtual ~FontAtlas();

        static FontAtlas* Create(Name name, CMFontAtlas* cmFontAtlas, Object* outer = nullptr);

        inline RPI::Texture* GetAtlasTexture() const { return atlas; }

        inline const Array<FontGlyphLayout>& GetGlyphLayouts() const { return glyphLayouts; }

        inline const FontGlyphLayout& GetGlyphLayout(u32 unicode) 
        { 
            if (glyphLayoutCache.IsEmpty())
                CacheGlyphLayouts();

            return glyphLayoutCache[unicode]; 
        }

        inline u32 GetWidth() const { return width; }

        inline u32 GetHeight() const { return height; }

    private:

        void OnAfterDeserialize() override;

        void CacheGlyphLayouts();

        RPI::Texture* atlas = nullptr;

        FIELD()
        u32 width = 0;

        FIELD()
        u32 height = 0;

        FIELD()
        Array<FontGlyphLayout> glyphLayouts{};

        HashMap<u32, FontGlyphLayout> glyphLayoutCache{};
    };

} // namespace CE::RPI

#include "FontAtlas.rtti.h"