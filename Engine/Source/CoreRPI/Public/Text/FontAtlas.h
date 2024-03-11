#pragma once

namespace CE::RPI
{

    CLASS()
    class CORERPI_API FontAtlas : public Object
    {
        CE_CLASS(FontAtlas, Object)
    public:

        virtual ~FontAtlas();

        static FontAtlas* Create(Name name, RPI::Texture* atlas, const FontAtlasLayout& atlasLayout, Object* outer = nullptr);

        inline RPI::Texture* GetAtlasTexture() const { return atlas; }

        inline const FontAtlasLayout& GetAtlasLayout() const { return atlasLayout; }

    private:

        RPI::Texture* atlas = nullptr;

        FIELD()
        FontAtlasLayout atlasLayout{};

    };

} // namespace CE::RPI

#include "FontAtlas.rtti.h"