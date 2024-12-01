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

        // - Data Structures -

        struct ImageItem
        {
            int layerIndex = -1;
            Vec2 uvMin, uvMax;

            bool IsValid() const { return layerIndex >= 0; }
        };

        struct FUSIONCORE_API BinaryNode : IntrusiveBase
        {
            StaticArray<Ptr<BinaryNode>, 2> child;
            BinaryNode* parent = nullptr;
            int totalChildren = 0;

            Rect rect;
            Name imageName;
            int imageId = -1;

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

            void ClearImage()
            {
                imageName = Name();
                imageId = -1;
            }

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

            Ptr<BinaryNode> FindUsedNode()
            {
                if (IsValid())
                    return this;

                if (child[0] != nullptr)
                {
                    Ptr<BinaryNode> node = child[0]->FindUsedNode();
                    if (node != nullptr)
                    {
                        return node;
                    }
                }

                if (child[1] != nullptr)
                {
                    Ptr<BinaryNode> node = child[1]->FindUsedNode();
                    if (node != nullptr)
                    {
                        return node;
                    }
                }

                return nullptr;
            }

            Ptr<BinaryNode> Insert(Vec2i imageSize);

            bool Defragment();
            bool DefragmentSlow();
        };

    private:

        // - Atlas -

        struct FAtlasImage : IntrusiveBase
        {
            virtual ~FAtlasImage()
            {
                delete ptr; ptr = nullptr;
            }

            u8* ptr = nullptr;
            u32 atlasSize = 0;
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
