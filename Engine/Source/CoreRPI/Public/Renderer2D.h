#pragma once

namespace CE::RPI
{
    class Shader;

    struct Renderer2DDescriptor
    {
        Vec2i screenSize{};
        RPI::Shader* textShader = nullptr;
    };
    
    class CORERPI_API Renderer2D
    {
    public:

        Renderer2D(const Renderer2DDescriptor& desc);
        virtual ~Renderer2D();

        void AddFont(Name name, RPI::FontAtlasAsset* fontAtlasData);

        void SetScreenSize(Vec2i newScreenSize);

    private:

        struct alignas(16) TextDrawItem
        {
            Matrix4x4 transform{};
            Vec4 atlasUV{};
            f32 bgMask = 0;
        };

        struct FontVariant
        {
            RPI::FontAtlasAsset* atlasData = nullptr;
            Color foreground = Color(1, 1, 1, 1);
            Color background = Color(0, 0, 0, 0);
            bool bold = false;

            inline SIZE_T GetHash() const
            {
                SIZE_T hash = (SIZE_T)atlasData;
                CombineHash(hash, foreground);
                CombineHash(hash, background);
                CombineHash(hash, bold);
                return hash;
            }

            inline bool operator==(const FontVariant& other) const
            {
                return GetHash() == other.GetHash();
            }
        };

        RPI::Shader* textShader = nullptr;

        Vec2i screenSize = Vec2i(0, 0);

        HashMap<Name, RPI::FontAtlasAsset*> fontAtlasesByName{};
        HashMap<FontVariant, RPI::Material*> materials{};
    };

} // namespace CE::RPI
