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

        // - Utils -

        bool AddImage(const Name& name, const CMImage& imageSource);

        // - Configs -

        FIELD(Config)
        u32 atlasSize = 4096;

        // - Data Structures -

        struct RowSegment {
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

        // - Internals -

        StaticArray<RPI::Texture*, RHI::Limits::MaxSwapChainImageCount> atlasTexturesPerImage;

        Array<Ptr<FAtlasImage>> atlasLayers;
        StaticArray<bool, RHI::Limits::MaxSwapChainImageCount> flushRequiredPerImage;

        HashMap<Name, RHI::TextureView*> texturesByName;

    };
    
} // namespace CE

#include "FImageAtlas.rtti.h"
