#pragma once

namespace CE::RPI
{
    struct Renderer2DDescriptor
    {
        HashMap<Name, RPI::FontAtlasAsset*> fonts{};
    };
    
    class CORERPI_API Renderer2D
    {
    public:

        Renderer2D(const Renderer2DDescriptor& desc);
        virtual ~Renderer2D();

    private:

        struct FontVariant
        {
            RPI::FontAtlasAsset* atlasData = nullptr;
            Color foreground = Color(1, 1, 1, 1);
            bool bold = false;
        };

    };

} // namespace CE::RPI
