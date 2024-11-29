#pragma once

namespace CE::RPI
{
    struct TextureAtlasDescriptor
    {
        Name name{};
        u32 width = 2048, height = 2048;
        RHI::Format format = RHI::Format::R8G8B8A8_UNORM;
        TextureBindFlags bindFlags = TextureBindFlags::ShaderRead;
        u32 arrayLayers = 1;

        RHI::SamplerDescriptor samplerDesc = {};

        BinaryBlob* source = nullptr;
    };

    class CORERPI_API TextureAtlas : public RPI::Texture
    {
    public:
        typedef RPI::Texture Super;
        typedef TextureAtlas Self;

        TextureAtlas(const TextureAtlasDescriptor& descriptor);

        virtual ~TextureAtlas();

    private:

        struct TextureItem
        {
            RHI::TextureView* textureView = nullptr;
        };

        struct RowSegment
        {
            int x, y;
            int height;
        };

        struct AtlasImage : IntrusiveBase
        {
            virtual ~AtlasImage()
            {

            }

            u32 atlasWidth = 0;
            u32 atlasHeight = 0;
            int arrayIndex = 0;
            Array<RowSegment> rows;

            HashMap<Name, TextureItem> texturesByName;

            bool FindInsertionPoint(Vec2i textureSize, int& outX, int& outY);
        };

        u32 numArrayLayers = 0;
        u32 width = 0, height = 0;

        Array<Ptr<AtlasImage>> atlasImages;

    };

}
