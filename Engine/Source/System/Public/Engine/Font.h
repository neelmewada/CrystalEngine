#pragma once

namespace CE
{
#if PAL_TRAIT_BUILD_TESTS
    namespace Editor { class FontAssetImportJob; }
#endif

    CLASS()
    class SYSTEM_API Font : public Asset
    {
        CE_CLASS(Font, Asset)
    public:

        virtual ~Font();

        inline CE::Texture2D* GetAtlasTexture() const { return fontAtlas; }

        const RPI::FontGlyphLayout& GetGlyphLayout(u32 unicode);

    private:

        void OnAfterDeserialize() override;

        void CacheGlyphLayouts();

        FIELD()
        CE::Texture2D* fontAtlas = nullptr;

        FIELD()
        Array<RPI::FontGlyphLayout> glyphLayouts{};

        HashMap<u32, RPI::FontGlyphLayout> glyphLayoutsCache{};

#if PAL_TRAIT_BUILD_TESTS
        friend class CE::Editor::FontAssetImportJob;
#endif
    };

} // namespace CE

#include "Font.rtti.h"