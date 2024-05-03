#include "CoreRPI.h"

namespace CE::RPI
{
    static constexpr f32 quadVertexData[] = {
        // Positions
        -1.0f, -1.0f, 0, 0,
        1.0f, -1.0f, 0, 0,
        -1.0f, 1.0f, 0, 0,
        -1.0f, 1.0f, 0, 0,
        1.0f, -1.0f, 0, 0,
        1.0f, 1.0f, 0, 0,
        // UVs
        0.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        0.0f, 1.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
    };

    static constexpr f32 textQuadVertexData[] = {
        // Positions
        0.0f, 0.0f, 0, 0,
        1.0f, 0.0f, 0, 0,
        0.0f, 1.0f, 0, 0,
        0.0f, 1.0f, 0, 0,
        1.0f, 0.0f, 0, 0,
        1.0f, 1.0f, 0, 0,
        // UVs
        0.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        0.0f, 1.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
    };

    RPISystem& RPISystem::Get()
    {
		static RPISystem instance{};
        return instance;
    }

    void RPISystem::Initialize()
    {
        isFirstTick = true;
        isInitialized = true;

        CreateDefaultTextures();
        CreateFullScreenQuad();

        PassSystem::Get().Initialize();

        builtinDrawTags.Clear();

        for (int i = 1; i < (int)BuiltinDrawItemTag::COUNT; ++i)
        {
            RHI::DrawListTag tag{};
            DrawListTagRegistry* registry = GetDrawListTagRegistry();

            switch ((BuiltinDrawItemTag)i)
            {
            case BuiltinDrawItemTag::Depth:
                tag = registry->AcquireTag("depth");
	            break;
            case BuiltinDrawItemTag::Opaque:
                tag = registry->AcquireTag("opaque");
	            break;
            case BuiltinDrawItemTag::Shadow:
                tag = registry->AcquireTag("shadow");
	            break;
            case BuiltinDrawItemTag::UI:
                tag = registry->AcquireTag("ui");
	            break;
            case BuiltinDrawItemTag::Transparent:
                tag = registry->AcquireTag("transparent");
                break;
            case BuiltinDrawItemTag::Skybox:
                tag = registry->AcquireTag("skybox");
                break;
            case BuiltinDrawItemTag::None:
            case BuiltinDrawItemTag::COUNT:
                continue;
            }
            builtinDrawTags[(BuiltinDrawItemTag)i] = tag;
        }
    }

    void RPISystem::PostInitialize(RPI::ShaderCollection* defaultShader)
    {
        this->defaultShader = defaultShader;

        if (!defaultShader)
            return;

        for (const ShaderCollection::Item& shaderItem : *defaultShader)
        {
	        if (!shaderItem.shader)
                continue;

            RPI::ShaderVariant* variant = shaderItem.shader->GetVariant(shaderItem.shader->GetDefaultVariantIndex());
            if (!variant)
                continue;

            viewSrgLayout = variant->GetSrgLayout(SRGType::PerView);
            sceneSrgLayout = variant->GetSrgLayout(SRGType::PerScene);
        }
    }

    void RPISystem::Shutdown()
    {
        defaultShader = nullptr;
        isInitialized = false;

        for (const auto& [builtinTag, drawListTag] : builtinDrawTags)
        {
            if (!drawListTag.IsValid())
                continue;
            GetDrawListTagRegistry()->ReleaseTag(drawListTag);
        }
        builtinDrawTags.Clear();

	    for (int i = (int)scenes.GetSize() - 1; i >= 0; --i)
	    {
			delete scenes[i];
	    }
        scenes.Clear();

        PassSystem::Get().Shutdown();

        for (auto buffer : vertexBuffers)
        {
            RHI::gDynamicRHI->DestroyBuffer(buffer);
        }
        vertexBuffers.Clear();

        delete defaultNormalTex; defaultNormalTex = nullptr;
        delete defaultAlbedoTex; defaultAlbedoTex = nullptr;
        delete defaultRoughnessTex; defaultRoughnessTex = nullptr;

        {
            LockGuard lock{ samplerCacheMutex };

            for (auto [_, sampler] : samplerCache)
            {
                RHI::gDynamicRHI->DestroySampler(sampler);
            }
            samplerCache.Clear();
        }
    }

    void RPISystem::SimulationTick(u32 imageIndex)
    {
        if (isFirstTick)
        {
            startTime = clock();
            isFirstTick = false;
        }

        currentTime = GetCurrentTime();

        for (Scene* scene : scenes)
        {
            scene->Simulate(currentTime);
        }
    }

    void RPISystem::RenderTick(u32 imageIndex)
    {
        for (Scene* scene : scenes)
        {
            scene->PrepareRender(currentTime, imageIndex);
        }

        for (Scene* scene : scenes)
        {
            scene->OnRenderEnd();
        }
    }

    f32 RPISystem::GetCurrentTime() const
    {
        return (f32)(clock() - startTime) / CLOCKS_PER_SEC;
    }

    void RPISystem::CreateFullScreenQuad()
    {
        RHI::BufferDescriptor quadVertexBufferDesc{};
        quadVertexBufferDesc.name = "Quad Vertex Buffer";
        quadVertexBufferDesc.bindFlags = RHI::BufferBindFlags::VertexBuffer;
        quadVertexBufferDesc.defaultHeapType = RHI::MemoryHeapType::Default;
        quadVertexBufferDesc.bufferSize = sizeof(quadVertexData);

        RHI::Buffer* quadVertexBuffer = RHI::gDynamicRHI->CreateBuffer(quadVertexBufferDesc);
        quadVertexBuffer->UploadData(quadVertexData, quadVertexBuffer->GetBufferSize(), 0);
        vertexBuffers.Add(quadVertexBuffer);

        quadVertexBufferViews.Add(RHI::VertexBufferView(quadVertexBuffer, 0, sizeof(Vec4) * 6, sizeof(Vec4)));
        quadVertexBufferViews.Add(RHI::VertexBufferView(quadVertexBuffer, sizeof(Vec4) * 6, sizeof(Vec2) * 6, sizeof(Vec2)));

        quadDrawArgs.firstInstance = 0;
        quadDrawArgs.instanceCount = 1;
        quadDrawArgs.vertexOffset = 0;
        quadDrawArgs.vertexCount = 6;

        RHI::Buffer* textQuadVertexBuffer = RHI::gDynamicRHI->CreateBuffer(quadVertexBufferDesc);
        textQuadVertexBuffer->UploadData(textQuadVertexData, textQuadVertexBuffer->GetBufferSize(), 0);
        vertexBuffers.Add(textQuadVertexBuffer);

        textQuadVertexBufferViews.Add(RHI::VertexBufferView(textQuadVertexBuffer, 0, sizeof(Vec4) * 6, sizeof(Vec4)));
        textQuadVertexBufferViews.Add(RHI::VertexBufferView(textQuadVertexBuffer, sizeof(Vec4) * 6, sizeof(Vec2) * 6, sizeof(Vec2)));

        textQuadDrawArgs.firstInstance = 0;
        textQuadDrawArgs.instanceCount = 1;
        textQuadDrawArgs.vertexOffset = 0;
        textQuadDrawArgs.vertexCount = 6;
    }

    RHI::Sampler* RPISystem::FindOrCreateSampler(const RHI::SamplerDescriptor& desc)
    {
        LockGuard<SharedMutex> lock{ samplerCacheMutex };

        SIZE_T hash = desc.GetHash();
        RHI::Sampler* sampler = samplerCache[hash];
        if (sampler != nullptr)
            return sampler;

        sampler = RHI::gDynamicRHI->CreateSampler(desc);
        samplerCache[hash] = sampler;
        return sampler;
    }

    void RPISystem::CreateDefaultTextures()
    {
        auto prevTime = clock();

        RHI::SamplerDescriptor samplerDesc{};
        samplerDesc.addressModeU = samplerDesc.addressModeV = samplerDesc.addressModeW = 
            SamplerAddressMode::Repeat;
        samplerDesc.samplerFilterMode = RHI::FilterMode::Linear;
        samplerDesc.enableAnisotropy = false;

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
        auto albedoTex = RHI::gDynamicRHI->CreateTexture(textureDesc);

        defaultAlbedoTex = new RPI::Texture(albedoTex, samplerDesc);

        textureDesc.name = "Default Normal";
        textureDesc.format = RHI::Format::R8G8B8A8_UNORM;
        auto normalTex = RHI::gDynamicRHI->CreateTexture(textureDesc);

        defaultNormalTex = new RPI::Texture(normalTex, samplerDesc);

        textureDesc.name = "Default Roughness";
        textureDesc.format = RHI::Format::R8_UNORM;
        auto roughnessTex = RHI::gDynamicRHI->CreateTexture(textureDesc);

        defaultRoughnessTex = new RPI::Texture(roughnessTex, samplerDesc);

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

            barrier.resource = albedoTex;
            barrier.fromState = ResourceState::Undefined;
            barrier.toState = ResourceState::CopyDestination;
            cmdList->ResourceBarrier(1, &barrier);

            barrier.resource = roughnessTex;
            barrier.fromState = ResourceState::Undefined;
            barrier.toState = ResourceState::CopyDestination;
            cmdList->ResourceBarrier(1, &barrier);

            barrier.resource = normalTex;
            barrier.fromState = ResourceState::Undefined;
            barrier.toState = ResourceState::CopyDestination;
            cmdList->ResourceBarrier(1, &barrier);

            // Albedo Copy
            RHI::BufferToTextureCopy copy{};
            copy.srcBuffer = stagingBuffer;
            copy.bufferOffset = 0;
            copy.dstTexture = albedoTex;
            copy.baseArrayLayer = 0;
            copy.layerCount = 1;
            copy.mipSlice = 0;
            cmdList->CopyTextureRegion(copy);

            // Roughness Copy
            copy.srcBuffer = stagingBuffer;
            copy.bufferOffset = roughnessOffset;
            copy.dstTexture = roughnessTex;
            cmdList->CopyTextureRegion(copy);

            // Normal copy
            copy.srcBuffer = stagingBuffer;
            copy.bufferOffset = normalOffset;
            copy.dstTexture = normalTex;
            cmdList->CopyTextureRegion(copy);
            
            barrier.resource = albedoTex;
            barrier.fromState = ResourceState::CopyDestination;
            barrier.toState = ResourceState::FragmentShaderResource;
            cmdList->ResourceBarrier(1, &barrier);

            barrier.resource = roughnessTex;
            barrier.fromState = ResourceState::CopyDestination;
            barrier.toState = ResourceState::FragmentShaderResource;
            cmdList->ResourceBarrier(1, &barrier);

            barrier.resource = normalTex;
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

        auto timeTaken = ((f32)(clock() - prevTime)) / CLOCKS_PER_SEC;
    }

} // namespace CE::RPI
