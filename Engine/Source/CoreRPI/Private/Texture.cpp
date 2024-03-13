#include "CoreRPI.h"

namespace CE::RPI
{
    static u64 CalculateTotalTextureSize(u32 width, u32 height, u32 bitsPerPixel, u32 arrayCount = 1, u32 mipLevelCount = 1)
    {
        u64 size = 0;

        for (int mip = 0; mip < mipLevelCount; mip++)
        {
            u32 power = (u32)pow(2, mip);
            u64 curSize = width / power * height / power * bitsPerPixel / 8 * arrayCount;
            size += curSize;
        }

        return size;
    }
    
    Texture::Texture(const TextureDescriptor& desc)
    {
        texture = RHI::gDynamicRHI->CreateTexture(desc.texture);

        samplerState = RPISystem::Get().FindOrCreateSampler(desc.samplerDesc);

        if (desc.source == nullptr)
        {
            return;
        }

        UploadData(desc.source->GetDataPtr(), desc.source->GetDataSize());
    }

    Texture::Texture(RHI::Texture* texture, const RHI::SamplerDescriptor& samplerDesc)
        : texture(texture)
    {
        samplerState = RPISystem::Get().FindOrCreateSampler(samplerDesc);
    }

    Texture::Texture(RHI::TextureView* textureView, const RHI::SamplerDescriptor& samplerDesc)
        : texture(nullptr)
        , textureView(textureView)
    {
        samplerState = RPISystem::Get().FindOrCreateSampler(samplerDesc);
    }

    Texture::Texture(const CMImage& sourceImage, const RHI::SamplerDescriptor& samplerDesc)
    {
        if (!sourceImage.IsValid())
            return;

        u8* data = (u8*)sourceImage.GetDataPtr();
        u32 dataSize = sourceImage.GetDataSize();

        RHI::TextureDescriptor desc{};
        desc.name = "CMImage";
        desc.bindFlags = RHI::TextureBindFlags::ShaderRead;
        desc.mipLevels = 1;
        desc.arrayLayers = 1;
        desc.width = sourceImage.GetWidth();
        desc.height = sourceImage.GetHeight();
        desc.depth = 1;
        desc.dimension = Dimension::Dim2D;
        desc.sampleCount = 1;

        switch (sourceImage.GetFormat())
        {
        case CMImageFormat::R8:
            desc.format = Format::R8_UNORM;
            break;
        case CMImageFormat::RG8:
            desc.format = Format::R8G8_UNORM;
            break;
        case CMImageFormat::RGB8:
            desc.format = Format::R8G8B8_UNORM;
            break;
        case CMImageFormat::RGBA8:
            desc.format = Format::R8G8B8A8_UNORM;
            break;
        case CMImageFormat::R16:
            desc.format = Format::R16_SFLOAT;
            break;
        case CMImageFormat::RG16:
            desc.format = Format::R16G16_SFLOAT;
            break;
        case CMImageFormat::RGBA16:
            desc.format = Format::R16G16B16A16_SFLOAT;
            break;
        default:
            return;
        }

        texture = RHI::gDynamicRHI->CreateTexture(desc);

        samplerState = RPISystem::Get().FindOrCreateSampler(samplerDesc);

        UploadData(data, dataSize);
    }

    Texture::~Texture()
    {
        delete texture;
        texture = nullptr;
        delete textureView;
        textureView = nullptr;
    }

    void Texture::UploadData(u8* src, u64 dataSize)
    {
        if (src == nullptr || dataSize == 0)
        {
            return;
        }

        int totalMipLevels = texture->GetMipLevelCount();

        RHI::CommandQueue* queue = RHI::gDynamicRHI->GetPrimaryTransferQueue();
        auto commandList = RHI::gDynamicRHI->AllocateCommandList(queue);
        auto uploadFence = RHI::gDynamicRHI->CreateFence(false);

        RHI::BufferDescriptor stagingDesc{};
        stagingDesc.name = "Staging Buffer";
        stagingDesc.bindFlags = RHI::BufferBindFlags::StagingBuffer;
        stagingDesc.bufferSize = dataSize;
        stagingDesc.defaultHeapType = RHI::MemoryHeapType::Upload;

        RHI::Buffer* stagingBuffer = RHI::gDynamicRHI->CreateBuffer(stagingDesc);
        stagingBuffer->UploadData(src, dataSize);

        commandList->Begin();
        {
            ResourceBarrierDescriptor barrier{};
            barrier.resource = stagingBuffer;
            barrier.fromState = ResourceState::Undefined;
            barrier.toState = ResourceState::CopySource;
            commandList->ResourceBarrier(1, &barrier);

            barrier.resource = texture;
            barrier.fromState = ResourceState::Undefined;
            barrier.toState = ResourceState::CopyDestination;
            commandList->ResourceBarrier(1, &barrier);

            u64 bufferOffset = 0;

            for (int mip = 0; mip < totalMipLevels; mip++)
            {
                RHI::BufferToTextureCopy copyRegion{};
                copyRegion.srcBuffer = stagingBuffer;
                copyRegion.bufferOffset = bufferOffset;
                copyRegion.dstTexture = texture;
                copyRegion.baseArrayLayer = 0;
                copyRegion.layerCount = texture->GetArrayLayerCount();
                copyRegion.mipSlice = mip;

                commandList->CopyTextureRegion(copyRegion);
                u32 power = (u32)pow(2, mip);
                
                bufferOffset += texture->GetWidth() / power * texture->GetHeight() / power * texture->GetBitsPerPixel() / 8 * copyRegion.layerCount;
            }

            barrier.resource = texture;
            barrier.fromState = ResourceState::CopyDestination;
            barrier.toState = ResourceState::FragmentShaderResource;
            commandList->ResourceBarrier(1, &barrier);
        }
        commandList->End();

        queue->Execute(1, &commandList, uploadFence);
        uploadFence->WaitForFence();

        delete stagingBuffer;
        RHI::gDynamicRHI->FreeCommandLists(1, &commandList);
        delete uploadFence;
    }

} // namespace CE::RPI
