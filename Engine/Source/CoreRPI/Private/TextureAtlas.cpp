#include "CoreRPI.h"

namespace CE::RPI
{

    TextureAtlas::TextureAtlas(const TextureAtlasDescriptor& descriptor)
        : RPI::Texture(RPI::TextureDescriptor{
            .texture = { .name = descriptor.name,
                .width = descriptor.width, .height = descriptor.height, .depth = 1, .dimension = Dimension::Dim2D,
                .format = descriptor.format, .mipLevels = 1, .sampleCount = 1,
                .arrayLayers = descriptor.arrayLayers,
                .bindFlags = descriptor.bindFlags,
                .defaultHeapType = MemoryHeapType::Default },
            .samplerDesc = descriptor.samplerDesc,
            .source = descriptor.source
        })
    {
        numArrayLayers = descriptor.arrayLayers;
        width = descriptor.width;;
        height = descriptor.height;

        u32 imageByteSize = width * height * numArrayLayers * sizeof(u32); // RGBA Channels
        Ptr<AtlasImage> atlas = new AtlasImage;
        atlas->atlasWidth = width;
        atlas->atlasHeight = height;
        atlas->arrayIndex = 0;

        atlasImages.Add(atlas);
    }

    TextureAtlas::~TextureAtlas()
    {
        
    }

    bool TextureAtlas::AtlasImage::FindInsertionPoint(Vec2i textureSize, int& outX, int& outY)
    {
        ZoneScoped;

        int bestRowIndex = -1;
        int bestRowHeight = INT_MAX;

        if (rows.IsEmpty())
        {
            rows.Add({ .x = textureSize.width, .y = 0, .height = textureSize.height });
            outX = 0;
            outY = 0;
            return true;
        }

        for (int i = 0; i < rows.GetSize(); ++i) 
        {
            int x = rows[i].x;
            int y = rows[i].y;

            // Check if the glyph fits at this position
            if (x + textureSize.width <= atlasWidth && y + textureSize.height <= atlasHeight)
            {
                if (rows[i].height >= textureSize.height && rows[i].height < bestRowHeight)
                {
                    bestRowHeight = rows[i].height;
                    bestRowIndex = i;
                }
            }
        }

        if (bestRowIndex == -1)
        {
            RowSegment lastRow = rows.Top();
            if (lastRow.y + lastRow.height + textureSize.height > atlasHeight)
            {
                return false;
            }

            rows.Add({ .x = textureSize.width, .y = lastRow.y + lastRow.height, .height = textureSize.height });

            outX = 0;
            outY = rows.Top().y;

            return true;
        }

        outX = rows[bestRowIndex].x;
        outY = rows[bestRowIndex].y;

        rows[bestRowIndex].x += textureSize.width;

        return true;
    }


}

