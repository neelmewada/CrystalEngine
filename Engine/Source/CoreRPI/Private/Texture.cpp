#include "CoreRPI.h"

namespace CE::RPI
{
    
    Texture::Texture(const TextureDescriptor& desc)
    {
        u8* src = desc.source->GetDataPtr();
        u64 dataSize = desc.source->GetDataSize();

        if (src == nullptr || dataSize == 0)
        {
            CE_LOG(Error, All, "Failed to create texture {}. source data is empty!", desc.texture.name);
            return;
        }

        texture = RHI::gDynamicRHI->CreateTexture(desc.texture);

        RPISystem::Get().FindOrCreateSampler(desc.samplerDesc);
        samplerState = RHI::gDynamicRHI->CreateSampler(desc.samplerDesc);

        RHI::CommandQueue* queue = RHI::gDynamicRHI->GetPrimaryGraphicsQueue();
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

            ResourceBarrierDescriptor barrier{};
            barrier.resource = texture;
            barrier.fromState = ResourceState::Undefined;
            barrier.toState = ResourceState::CopyDestination;
            commandList->ResourceBarrier(1, &barrier);

            RHI::BufferToTextureCopy copyRegion{};
            copyRegion.

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

    Texture::~Texture()
    {
        delete texture;
        texture = nullptr;
    }

} // namespace CE::RPI
