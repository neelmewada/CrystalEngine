#pragma once

namespace CE::RPI
{
    class TextureAsset;

    CLASS()
    class CORERPI_API FontAtlasAsset : public Object
    {
        CE_CLASS(FontAtlasAsset, Object)
    public:

        virtual ~FontAtlasAsset();

        inline const Array<FontGlyphLayout>& GetGlyphLayouts() const { return glyphLayouts; }

        inline const FontGlyphLayout& GetGlyphLayout(u32 unicode) 
        { 
            if (glyphLayoutCache.IsEmpty())
                CacheGlyphLayouts();

            return glyphLayoutCache[unicode]; 
        }

        inline u32 GetWidth() const { return fontAtlasTexture != nullptr ? fontAtlasTexture->width : 0; }

        inline u32 GetHeight() const { return fontAtlasTexture != nullptr ? fontAtlasTexture->height : 0; }

        inline TextureAsset* GetAtlasTexture() const { return fontAtlasTexture; }

    private:

        void OnAfterDeserialize() override;

        void CacheGlyphLayouts();

        FIELD()
        TextureAsset* fontAtlasTexture = nullptr;

        FIELD()
        Array<FontGlyphLayout> glyphLayouts{};

        HashMap<u32, FontGlyphLayout> glyphLayoutCache{};

#if PAL_TRAIT_BUILD_EDITOR
        friend class CE::Editor::FontAssetImportJob;
#endif
    };

} // namespace CE::RPI

#include "FontAtlas.rtti.h"