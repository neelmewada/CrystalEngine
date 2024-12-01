#include "FusionCore.h"

namespace CE
{

    FImageAtlas::FImageAtlas()
    {

    }

    FImageAtlas::~FImageAtlas()
    {
        for (int i = 0; i < atlasTexturesPerImage.GetSize(); ++i)
        {
            delete atlasTexturesPerImage[i];
            atlasTexturesPerImage[i] = nullptr;
        }
        atlasLayers.Clear();
    }

    void FImageAtlas::Init()
    {
        ZoneScoped;

        if (atlasLayers.NotEmpty())
            return;

        Ptr<FAtlasImage> atlas = new FAtlasImage;
        atlas->ptr = new u8[atlasSize * atlasSize * sizeof(u32)];
        memset(atlas->ptr, 0, atlasSize * atlasSize * sizeof(u32));
        atlas->atlasSize = atlasSize;
        atlas->layerIndex = 0;

        atlasLayers.Add(atlas);

        RPI::TextureDescriptor textureDescriptor{};
        textureDescriptor.texture.width = textureDescriptor.texture.height = atlasSize;
        textureDescriptor.texture.sampleCount = 1;
        textureDescriptor.texture.depth = 1;
        textureDescriptor.texture.dimension = Dimension::Dim2D;
        textureDescriptor.texture.arrayLayers = atlasLayers.GetSize();
        textureDescriptor.texture.name = "Fusion Image Atlas";
        textureDescriptor.texture.mipLevels = 1;
        textureDescriptor.texture.bindFlags = TextureBindFlags::ShaderRead;
        textureDescriptor.texture.format = Format::R8G8B8A8_UNORM;
        textureDescriptor.texture.defaultHeapType = MemoryHeapType::Upload;

        textureDescriptor.samplerDesc.addressModeU = SamplerAddressMode::ClampToBorder;
        textureDescriptor.samplerDesc.addressModeV = SamplerAddressMode::ClampToBorder;
        textureDescriptor.samplerDesc.addressModeW = SamplerAddressMode::ClampToBorder;
        textureDescriptor.samplerDesc.borderColor = SamplerBorderColor::FloatTransparentBlack;
        textureDescriptor.samplerDesc.enableAnisotropy = false;
        textureDescriptor.samplerDesc.samplerFilterMode = FilterMode::Linear;

        for (int i = 0; i < atlasTexturesPerImage.GetSize(); ++i)
        {
            atlasTexturesPerImage[i] = new RPI::Texture(textureDescriptor);
        }
    }

    void FImageAtlas::Flush(u32 imageIndex)
    {
        ZoneScoped;

        if (!flushRequiredPerImage[imageIndex])
            return;

        
    }

    FImageAtlas::ImageItem FImageAtlas::AddImage(const Name& name, const CMImage& imageSource)
    {
        if (!name.IsValid() || !imageSource.IsValid() || imagesByName.KeyExists(name))
        {
            return {};
        }

        bool foundEmptySlot = false;
        Vec2i textureSize = Vec2i(imageSource.GetWidth(), imageSource.GetHeight());
        int posX = -1, posY = -1;
        Ptr<FAtlasImage> foundAtlas = nullptr;

        u32 srcPixelSize = sizeof(u32);
        switch (imageSource.GetFormat())
        {
        case CMImageFormat::Undefined:
            return {};
        case CMImageFormat::R8:
            srcPixelSize = sizeof(u8);
            break;
        case CMImageFormat::RG8:
            srcPixelSize = sizeof(u8) * 2;
            break;
        case CMImageFormat::RGB8:
        case CMImageFormat::RGBA8:
            srcPixelSize = sizeof(u8) * 4;
            break;
        case CMImageFormat::R32:
            srcPixelSize = sizeof(f32);
            break;
        case CMImageFormat::RG32:
            srcPixelSize = sizeof(f32) * 2;
            break;
        case CMImageFormat::RGB32:
            srcPixelSize = sizeof(f32) * 4;
            break;
        case CMImageFormat::RGBA32:
            srcPixelSize = sizeof(f32) * 4;
            break;
        case CMImageFormat::R16:
            srcPixelSize = sizeof(u16);
            break;
        case CMImageFormat::RG16:
            srcPixelSize = sizeof(u16) * 2;
            break;
        case CMImageFormat::RGB16:
            srcPixelSize = sizeof(u16) * 4;
            break;
        case CMImageFormat::RGBA16:
            srcPixelSize = sizeof(u16) * 4;
            break;
        }

        for (int i = 0; i < atlasLayers.GetSize(); ++i)
        {
            Ptr<FAtlasImage> atlas = atlasLayers[i];

            foundEmptySlot = atlas->FindInsertionPoint(textureSize, posX, posY);

            if (foundEmptySlot)
            {
                foundAtlas = atlas;
	            break;
            }
        }

        if (!foundEmptySlot)
        {
            Ptr<FAtlasImage> atlas = new FAtlasImage;
            atlas->ptr = new u8[atlasSize * atlasSize * sizeof(u32)];
            memset(atlas->ptr, 0, atlasSize * atlasSize * sizeof(u32));
            atlas->atlasSize = atlasSize;
            atlas->layerIndex = atlasLayers.GetSize();

            atlasLayers.Add(atlas);

            foundEmptySlot = atlas->FindInsertionPoint(textureSize, posX, posY);

            if (!foundEmptySlot)
            {
                return {};
            }
            else
            {
                foundAtlas = atlas;
            }
        }

        Vec2 uvMin = Vec2((f32)posX / (f32)foundAtlas->atlasSize, (f32)posY / (f32)foundAtlas->atlasSize);
        Vec2 uvMax = Vec2((f32)(posX + textureSize.width) / (f32)foundAtlas->atlasSize, (f32)(posY + textureSize.height) / (f32)foundAtlas->atlasSize);

        for (int x = 0; x < textureSize.x; ++x)
        {
	        for (int y = 0; y < textureSize.y; ++y)
	        {
                Vec2i dstPos = Vec2i(posX + x, posY + y);

                u8* dstPixel = foundAtlas->ptr + (foundAtlas->atlasSize * dstPos.y + dstPos.x) * sizeof(u32);
                u8* srcPixel = (u8*)imageSource.GetDataPtr() + (textureSize.y * y + x) * srcPixelSize;
                u8* r = dstPixel;
                u8* g = dstPixel + 1;
                u8* b = dstPixel + 2;
                u8* a = dstPixel + 3;
                
                switch (imageSource.GetFormat())
                {
                case CMImageFormat::Undefined:
	                break;
                case CMImageFormat::R8:
                    *r = *g = *b = *srcPixel;
                    *a = (u8)255;
	                break;
                case CMImageFormat::RG8:
                    *r = *g = *b = *srcPixel;
                    *a = *(srcPixel + 1);
	                break;
                case CMImageFormat::RGB8:
                    *r = *srcPixel;
                    *g = *(srcPixel + 1);
                    *b = *(srcPixel + 2);
                    *a = (u8)255;
	                break;
                case CMImageFormat::RGBA8:
                    *r = *srcPixel;
                    *g = *(srcPixel + 1);
                    *b = *(srcPixel + 2);
                    *a = *(srcPixel + 3);
	                break;
                case CMImageFormat::R32:
                    *r = *g = *b = static_cast<u8>(*((f32*)srcPixel) * 255.0f);
                    *a = (u8)255;
	                break;
                case CMImageFormat::RG32:
                    *r = *g = *b = static_cast<u8>(*((f32*)srcPixel) * 255.0f);
                    *a = static_cast<u8>(*((f32*)srcPixel + 1) * 255.0f);
	                break;
                case CMImageFormat::RGB32:
                    *r = static_cast<u8>(*((f32*)srcPixel) * 255.0f);
                    *g = static_cast<u8>(*((f32*)srcPixel + 1) * 255.0f);
                    *b = static_cast<u8>(*((f32*)srcPixel + 2) * 255.0f);
                    *a = (u8)255;
	                break;
                case CMImageFormat::RGBA32:
                    *r = static_cast<u8>(*((f32*)srcPixel) * 255.0f);
                    *g = static_cast<u8>(*((f32*)srcPixel + 1) * 255.0f);
                    *b = static_cast<u8>(*((f32*)srcPixel + 2) * 255.0f);
                    *a = static_cast<u8>(*((f32*)srcPixel + 3) * 255.0f);
	                break;
                case CMImageFormat::R16:
	                break;
                case CMImageFormat::RG16:
	                break;
                case CMImageFormat::RGB16:
	                break;
                case CMImageFormat::RGBA16:
	                break;
                }
	        }
        }

        ImageItem item = { .layerIndex = foundAtlas->layerIndex, .uvMin = uvMin, .uvMax = uvMax };

        imagesByName[name] = item;

        return item;
    }

    bool FImageAtlas::FAtlasImage::FindInsertionPoint(Vec2i textureSize, int& outX, int& outY)
    {
        ZoneScoped;

        

        return true;
    }

    Ptr<FImageAtlas::BinaryNode> FImageAtlas::BinaryNode::Insert(Vec2i imageSize)
    {
        using BinaryNode = FImageAtlas::BinaryNode;

        if (child[0] != nullptr)
        {
            // We are not in leaf node
            Ptr<BinaryNode> newNode = child[0]->Insert(imageSize);
            if (newNode != nullptr)
            {
                totalChildren++;
                return newNode;
            }
            if (child[1] == nullptr)
            {
                return nullptr;
            }

            newNode = child[1]->Insert(imageSize);
            if (newNode != nullptr)
            {
                totalChildren++;
            }

            return newNode;
        }
        else // We are in leaf node
        {
            if (IsValid()) // Do not split a valid node
                return nullptr;

            if (GetSize().width < imageSize.width ||
                GetSize().height < imageSize.height)
            {
                return nullptr;
            }

            if (GetSize().width == imageSize.width && GetSize().height == imageSize.height)
            {
                return this;
            }

            // Split the node
            child[0] = new BinaryNode;
            child[1] = new BinaryNode;
            child[0]->parent = this;
            child[1]->parent = this;

            totalChildren = 2;

            int dw = GetSize().width - imageSize.width;
            int dh = GetSize().height - imageSize.height;

            if (dw > dh)
            {
                child[0]->rect = Rect(rect.left, rect.top,
                    rect.left + imageSize.width, rect.bottom);
                child[1]->rect = Rect(rect.left + imageSize.width + 1, rect.top,
                    rect.right, rect.bottom);
            }
            else
            {
                child[0]->rect = Rect(rect.left, rect.top,
                    rect.right, rect.top + imageSize.height);
                child[1]->rect = Rect(rect.left, rect.top + imageSize.height + 1,
                    rect.right, rect.bottom);
            }

            return child[0]->Insert(imageSize);
        }
    }

    bool FImageAtlas::BinaryNode::Defragment()
    {
        if (child[0] != nullptr && child[1] != nullptr)
        {
            bool leftValid = child[0]->Defragment();
            bool rightValid = child[1]->Defragment();

            if (!leftValid && !rightValid)
            {
                child[0] = nullptr;
                child[1] = nullptr;

                return false;
            }

            // Better defragmentation but very slow:
            //if (false)
            {
                // TODO: It destroys valid rects too. Need to be fixed
                if (IsWidthSpan() && !leftValid && child[1]->child[0] != nullptr &&
                    child[1]->IsWidthSpan() && !child[1]->child[0]->IsValidRecursive())
                {
                    Ptr<BinaryNode> nodeToMove = child[1]->child[1];
                    child[0]->rect.max.x = child[1]->child[0]->rect.max.x;
                    child[1] = nodeToMove;
                    nodeToMove->parent = this;
                }
                else if (IsHeightSpan() && !leftValid && child[1]->child[0] != nullptr &&
                    child[1]->IsHeightSpan() && !child[1]->child[0]->IsValidRecursive())
                {
                    Ptr<BinaryNode> nodeToMove = child[1]->child[1];
                    child[0]->rect.max.y = child[1]->child[0]->rect.max.y;
                    child[1] = nodeToMove;
                    nodeToMove->parent = this;
                }
            }

            return true;
        }

        return IsValid();
    }

    FUSIONCORE_API bool atlasDefragmentDone = false;

    bool FImageAtlas::BinaryNode::DefragmentSlow()
    {
        if (child[0] != nullptr && child[1] != nullptr)
        {
            bool leftValid = child[0]->DefragmentSlow();
            bool rightValid = child[1]->DefragmentSlow();

            if (!leftValid && !rightValid)
            {
                return false;
            }

            // Better defragmentation but very slow:
            if (!atlasDefragmentDone)
            {
                if (IsWidthSpan() && !leftValid && child[1]->child[0] != nullptr &&
                    child[1]->IsHeightSpan() && !child[1]->child[0]->IsValidRecursive() &&
                    child[1]->child[1] != nullptr && child[1]->child[1]->IsWidthSpan() &&
                    child[1]->child[1]->child[0] != nullptr && !child[1]->child[1]->child[0]->IsValidRecursive())
                {
                    Ptr<BinaryNode> nodeToMove = child[1]->child[1]->child[1];
                    f32 splitX = child[1]->child[1]->child[0]->rect.max.x;

                    child[0]->rect.max.x = splitX;
                    child[1]->rect.min.x = splitX + 1;
                    child[1]->child[0]->rect.min.x = splitX + 1;
                    child[1]->child[1] = nodeToMove;
                    nodeToMove->parent = child[1]->child[1];

                    if (nodeToMove->IsHeightSpan() && !nodeToMove->child[0]->IsValidRecursive())
                    {
                    	Ptr<BinaryNode> contentNode = nodeToMove->child[1];
                        child[1]->child[0]->rect.max.y = contentNode->rect.min.y - 1;
                        child[1]->child[1] = contentNode;
                        contentNode->parent = child[1]->child[1];
                    }

                    //atlasDefragmentDone = true;
                }
            }

            return true;
        }

        return IsValid();
    }
} // namespace CE

