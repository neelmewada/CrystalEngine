#include "CoreRPI.h"

namespace CE::RPI
{

    RPISystem& RPISystem::Get()
    {
		static RPISystem instance{};
        return instance;
    }

    void RPISystem::Initialize()
    {
        CreateDefaultTextures();
    }

    void RPISystem::Shutdown()
    {
        delete defaultNormalTex; defaultNormalTex = nullptr;
        delete defaultAlbedoTex; defaultAlbedoTex = nullptr;
        delete defaultRoughnessTex; defaultRoughnessTex = nullptr;
    }

    void RPISystem::CreateDefaultTextures()
    {
        RHI::TextureDescriptor textureDesc{};
        textureDesc.name = "Default Albedo";
        textureDesc.arrayLayers = 1;
        textureDesc.mipLevels = 1;
        textureDesc.dimension = RHI::Dimension::Dim2D;
        textureDesc.width = 4;
        textureDesc.height = 4;
        textureDesc.depth = 1;
        textureDesc.defaultHeapType = RHI::MemoryHeapType::Default;
        textureDesc.sampleCount = 1;
        textureDesc.bindFlags = RHI::TextureBindFlags::ShaderRead;
        textureDesc.format = RHI::Format::R8G8B8A8_UNORM;
        defaultAlbedoTex = RHI::gDynamicRHI->CreateTexture(textureDesc);

        textureDesc.name = "Default Normal";
        textureDesc.format = RHI::Format::R8G8B8A8_UNORM;
        defaultNormalTex = RHI::gDynamicRHI->CreateTexture(textureDesc);

        textureDesc.name = "Default Roughness";
        textureDesc.format = RHI::Format::R8_UNORM;
        defaultRoughnessTex = RHI::gDynamicRHI->CreateTexture(textureDesc);

        const int numPixels = textureDesc.width * textureDesc.height;
        const u64 albedoSize = textureDesc.width * textureDesc.height * 4;
        const u64 normalSize = textureDesc.width * textureDesc.height * 4;
        const u64 roughnessSize = textureDesc.width * textureDesc.height;

        const u64 normalOffset = albedoSize;
        const u64 roughnessOffset = albedoSize + normalSize;

        RHI::BufferDescriptor stagingDesc{};
        stagingDesc.name = "Texture Copy Buffer";
        stagingDesc.bufferSize = albedoSize + roughnessSize + normalSize;
        stagingDesc.bindFlags = RHI::BufferBindFlags::StagingBuffer;
        stagingDesc.defaultHeapType = RHI::MemoryHeapType::Upload;

        RHI::Buffer* stagingBuffer = RHI::gDynamicRHI->CreateBuffer(stagingDesc);

        void* ptr = nullptr;
        stagingBuffer->Map(0, stagingBuffer->GetBufferSize(), &ptr);
        {
            u8* albedoData = (u8*)ptr;
            u8* normalData = (u8*)ptr + normalOffset;
            u8* roughnessData = (u8*)ptr + roughnessOffset;

            constexpr u8 u8Max = NumericLimits<u8>::Max();

            for (int i = 0; i < numPixels; i++)
            {
                *(albedoData + i * 4) = u8Max;
                *(albedoData + i * 4 + 1) = u8Max;
                *(albedoData + i * 4 + 2) = u8Max;
                *(albedoData + i * 4 + 3) = u8Max;

                *(normalData + i * 4) = (u8)127; // R
                *(normalData + i * 4 + 1) = (u8)127; // G
                *(normalData + i * 4 + 2) = (u8)255; // B
                *(normalData + i * 4 + 3) = 0; // A

                *(roughnessData + i) = u8Max;
            }
        }
        stagingBuffer->Unmap();

        RHI::CommandQueue* queue = RHI::gDynamicRHI->GetPrimaryGraphicsQueue();
        RHI::CommandList* cmdList = RHI::gDynamicRHI->AllocateCommandList(queue);
        RHI::Fence* fence = RHI::gDynamicRHI->CreateFence(false);

        cmdList->Begin();
        {
            RHI::ResourceBarrierDescriptor barrier{};
            barrier.resource = stagingBuffer;
            barrier.fromState = ResourceState::Undefined;
            barrier.toState = ResourceState::CopySource;
            cmdList->ResourceBarrier(1, &barrier);

            barrier.resource = defaultAlbedoTex;
            barrier.fromState = ResourceState::Undefined;
            barrier.toState = ResourceState::CopyDestination;
            cmdList->ResourceBarrier(1, &barrier);

            barrier.resource = defaultRoughnessTex;
            barrier.fromState = ResourceState::Undefined;
            barrier.toState = ResourceState::CopyDestination;
            cmdList->ResourceBarrier(1, &barrier);

            barrier.resource = defaultNormalTex;
            barrier.fromState = ResourceState::Undefined;
            barrier.toState = ResourceState::CopyDestination;
            cmdList->ResourceBarrier(1, &barrier);

            // Albedo Copy
            RHI::BufferToTextureCopy copy{};
            copy.srcBuffer = stagingBuffer;
            copy.bufferOffset = 0;
            copy.dstTexture = defaultAlbedoTex;
            copy.baseArrayLayer = 0;
            copy.layerCount = 1;
            copy.mipSlice = 0;
            cmdList->CopyTextureRegion(copy);

            // Roughness Copy
            copy.srcBuffer = stagingBuffer;
            copy.bufferOffset = roughnessOffset;
            copy.dstTexture = defaultRoughnessTex;
            cmdList->CopyTextureRegion(copy);

            // Normal copy
            copy.srcBuffer = stagingBuffer;
            copy.bufferOffset = normalOffset;
            copy.dstTexture = defaultNormalTex;
            cmdList->CopyTextureRegion(copy);
            
            barrier.resource = defaultAlbedoTex;
            barrier.fromState = ResourceState::CopyDestination;
            barrier.toState = ResourceState::FragmentShaderResource;
            cmdList->ResourceBarrier(1, &barrier);

            barrier.resource = defaultRoughnessTex;
            barrier.fromState = ResourceState::CopyDestination;
            barrier.toState = ResourceState::FragmentShaderResource;
            cmdList->ResourceBarrier(1, &barrier);

            barrier.resource = defaultNormalTex;
            barrier.fromState = ResourceState::CopyDestination;
            barrier.toState = ResourceState::FragmentShaderResource;
            cmdList->ResourceBarrier(1, &barrier);
        }
        cmdList->End();

        queue->Execute(1, &cmdList, fence);
        fence->WaitForFence();

        // Cleanup
        RHI::gDynamicRHI->FreeCommandLists(1, &cmdList);
        delete stagingBuffer; stagingBuffer = nullptr;
        delete fence; fence = nullptr;
    }

} // namespace CE::RPI
