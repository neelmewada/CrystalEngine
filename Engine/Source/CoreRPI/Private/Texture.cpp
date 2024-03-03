#include "CoreRPI.h"

namespace CE::RPI
{
    
    Texture::Texture(const TextureDescriptor& desc)
    {
        texture = RHI::gDynamicRHI->CreateTexture(desc.texture);

        samplerState = RPISystem::Get().FindOrCreateSampler(desc.samplerDesc);

        if (desc.source == nullptr)
        {
            return;
        }

        UploadData(desc.source);
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

    Texture::~Texture()
    {
        delete texture;
        texture = nullptr;
        delete textureView;
        textureView = nullptr;
    }

    void Texture::UploadData(BinaryBlob* source, int totalMipLevels)
    {
        if (source == nullptr)
            return;

        u8* src = (u8*)source->GetDataPtr();
        u64 dataSize = source->GetDataSize();

        if (src == nullptr || dataSize == 0)
        {
            return;
        }

        if (totalMipLevels <= 0)
        {
            totalMipLevels = texture->GetMipLevelCount();
        }

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

            for (int mip = 0; mip < texture->GetMipLevelCount(); mip++)
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
