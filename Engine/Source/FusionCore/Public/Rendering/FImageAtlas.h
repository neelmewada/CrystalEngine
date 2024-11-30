#pragma once

namespace CE
{
    CLASS(Config = Engine)
    class FUSIONCORE_API FImageAtlas : public Object
    {
        CE_CLASS(FImageAtlas, Object)
    protected:

        FImageAtlas();

        virtual ~FImageAtlas();

    public:

        // - Public API -

        void Init();

        //! @brief Flushes all the changes to GPU
        void Flush(u32 imageIndex);

    private:

        // - Data Structures -

        struct ImageItem
        {
            int layerIndex = -1;
            Vec2 uvMin, uvMax;

            bool IsValid() const { return layerIndex >= 0; }
        };

        struct RowSegment
    	{
            int x, y;
            int height;
        };

        struct FAtlasImage : IntrusiveBase
        {
            virtual ~FAtlasImage()
            {
                delete ptr; ptr = nullptr;
            }

            u8* ptr = nullptr;
            u32 atlasSize = 0;
            Array<RowSegment> rows;
            int layerIndex = 0;

            bool FindInsertionPoint(Vec2i textureSize, int& outX, int& outY);
        };

        // - Utils -

        ImageItem AddImage(const Name& name, const CMImage& imageSource);

        // - Configs -

        FIELD(Config)
        u32 atlasSize = 4096;

        // - Internals -

        StaticArray<RPI::Texture*, RHI::Limits::MaxSwapChainImageCount> atlasTexturesPerImage;

        Array<Ptr<FAtlasImage>> atlasLayers;
        StaticArray<bool, RHI::Limits::MaxSwapChainImageCount> flushRequiredPerImage;

        // - Cache -

        HashMap<Name, ImageItem> imagesByName;

    };
    
} // namespace CE

#include "FImageAtlas.rtti.h"
