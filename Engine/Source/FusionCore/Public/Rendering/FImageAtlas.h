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

        struct ImageItem;

        // - Getters -

        u32 GetAtlasSize() const { return atlasSize; }

        u32 GetArrayLayers() const { return atlasLayers.GetSize(); }

        RHI::ShaderResourceGroup* GetTextureSrg() const { return textureSrg; }

        Vec2 GetWhitePixelUV() const;
        Vec2 GetTransparentPixelUV() const;

        ImageItem FindImage(const Name& imageName);

        // - Public API -

        void Init();
        void Shutdown();

        //! @brief Flushes all the changes to GPU.
        void Flush(u32 imageIndex);

        // - Data Structures -

        struct ImageItem
        {
            int layerIndex = -1;
            Vec2 uvMin, uvMax;
            u32 width = 0, height = 0;

            bool IsValid() const { return layerIndex >= 0; }
        };

        struct FUSIONCORE_API BinaryNode : IntrusiveBase
        {
            StaticArray<Ptr<BinaryNode>, 2> child;
            BinaryNode* parent = nullptr;
            int totalChildren = 0;
            u32 usedArea = 0;

            Rect rect;
            Name imageName;
            // For debug use only!
            int imageId = -1;

            u32 GetTotalArea() const
            {
                return Math::RoundToInt(rect.GetSize().width * rect.GetSize().height);
            }

            u32 GetFreeArea() const
            {
                return GetTotalArea() - usedArea;
            }

            bool IsLeaf() const
            {
                return child[0] == nullptr && child[1] == nullptr;
            }

            bool IsWidthSpan() const
            {
                return !IsLeaf() && child[0]->rect.max.x < child[1]->rect.max.x;
            }

            bool IsHeightSpan() const
            {
                return !IsLeaf() && child[0]->rect.max.y < child[1]->rect.max.y;
            }

            bool IsValid() const
            {
                return imageName.IsValid();
            }

            bool IsValidRecursive() const
            {
                return IsValid() || (child[0] != nullptr && child[0]->IsValidRecursive()) ||
                    (child[1] != nullptr && child[1]->IsValidRecursive());
            }

            Vec2i GetSize() const
            {
                return Vec2i(Math::RoundToInt(rect.GetSize().width), Math::RoundToInt(rect.GetSize().height));
            }

            void ClearImage();

            template<typename TFunc>
            void ForEachRecursive(const TFunc& visitor)
            {
                visitor(this);

                if (child[0] != nullptr)
                {
                    child[0]->ForEachRecursive(visitor);
                }
                if (child[1] != nullptr)
                {
                    child[1]->ForEachRecursive(visitor);
                }
            }

            Ptr<BinaryNode> FindUsedNode();

            Ptr<BinaryNode> Insert(Vec2i imageSize);

            bool Defragment();
            bool DefragmentSlow();
        };

        // - Atlas -

        struct FAtlasImage : IntrusiveBase
        {
            FAtlasImage(u32 atlasSize) : atlasSize(atlasSize)
            {
                layerIndex = 0;
                root = new BinaryNode;

                root->rect = Rect(0, 0, atlasSize, atlasSize);
            }

            virtual ~FAtlasImage()
            {
                root = nullptr;
            }

            u32 atlasSize = 0;
            int layerIndex = 0;

            Ptr<BinaryNode> root = nullptr;
            HashMap<Name, Ptr<BinaryNode>> nodesByImageName;
        };

        // - Utils API -

        ImageItem AddImage(const Name& name, const CMImage& imageSource);
        bool RemoveImage(const Name& name);

        void UpdateImageAtlasItems();

        // - Model -

        MODEL_PROPERTY(Array<String>, Pages);

    private:

        // - Configs -

        FIELD(Config)
        u32 atlasSize = 4096;

        // - Internals -

        RHI::Buffer* stagingBuffer = nullptr;
        RHI::Fence* stagingBufferFence = nullptr;
        RHI::CommandList* stagingCommandList = nullptr;
        RHI::CommandQueue* stagingCommandQueue = nullptr;

        RHI::ShaderResourceGroup* textureSrg = nullptr;
        StaticArray<RPI::Texture*, RHI::Limits::MaxSwapChainImageCount> atlasTexturesPerFrame;

        Array<Ptr<FAtlasImage>> atlasLayers;
        StaticArray<bool, RHI::Limits::MaxSwapChainImageCount> flushRequiredPerImage;

        // - Cache -

        HashMap<Name, ImageItem> imagesByName;
        ImageItem whitePixel{};
        ImageItem transparentPixel{};

    };
    
} // namespace CE

#include "FImageAtlas.rtti.h"
