#include "FusionCore.h"

namespace CE
{
    static FusionApplication* gInstance = nullptr;

    extern RawData GetFusionShaderVert();
    extern RawData GetFusionShaderFrag();
    extern RawData GetFusionShaderVertJson();
    extern RawData GetFusionShaderFragJson();

    FusionApplication::FusionApplication()
    {
        fontManager = CreateDefaultSubobject<FFontManager>("FontManager");
        styleManager = CreateDefaultSubobject<FStyleManager>("StyleManager");
        rootContext = CreateDefaultSubobject<FRootContext>("RootContext");
    }

    FusionApplication::~FusionApplication()
    {
        if (gInstance == this)
        {
            gInstance = nullptr;
        }
    }

    FusionApplication* FusionApplication::Get()
    {
        if (gInstance == nullptr)
        {
            gInstance = CreateObject<FusionApplication>(GetTransient(MODULE_NAME), "FusionApplication");
        }
        return gInstance;
    }

    FusionApplication* FusionApplication::TryGet()
    {
        return gInstance;
    }

    void FusionApplication::Initialize(const FusionInitInfo& initInfo)
    {
        PlatformApplication::Get()->AddMessageHandler(this);

        InitializeShaders();
        
        fontManager->Init();

        IO::Path engineResourceDir = PlatformDirectories::GetEngineRootDir() / "Engine/Resources/Icons";
        engineResourceDir.RecursivelyIterateChildren([this](const IO::Path& path)
            {
                if (path.IsDirectory())
                    return;

				if (path.GetExtension() == ".png" || path.GetExtension() == ".jpg")
				{
                    LoadImageResource(path, path.GetFileName().RemoveExtension().GetString());
				}
            });

        imageRegistryUpdated = true;
    }

    void FusionApplication::PreShutdown()
    {
        PlatformApplication::Get()->RemoveMessageHandler(this);

        FrameScheduler* scheduler = FrameScheduler::Get();

        // Pre-shutdown and cleanup resources before engine assets are unloaded

        if (scheduler)
        {
            scheduler->WaitUntilIdle();
        }

        for (int i = 0; i < loadedTextures.GetCount(); ++i)
        {
            delete loadedTextures[i];
        }
        loadedTextures.RemoveAll();

        delete textureSrg; textureSrg = nullptr;
        fontManager->Shutdown();

        for (int i = destructionQueue.GetSize() - 1; i >= 0; --i)
        {
            destructionQueue[i].object->Destroy();
            destructionQueue.RemoveAt(i);
        }

        rootContext->Destroy();
    }

    void FusionApplication::Shutdown()
    {
        delete fusionShader; fusionShader = nullptr;
    }

    void FusionApplication::PushCursor(SystemCursor cursor)
    {
        cursorStack.Insert(cursor);
        PlatformApplication::Get()->SetSystemCursor(cursorStack.Last());
    }

    SystemCursor FusionApplication::GetCursor()
    {
        if (cursorStack.IsEmpty())
            return SystemCursor::Default;
        return cursorStack.Last();
    }

    void FusionApplication::PopCursor()
    {
        if (cursorStack.IsEmpty())
            return;

        cursorStack.RemoveLast();

        if (cursorStack.IsEmpty())
        {
            PlatformApplication::Get()->SetSystemCursor(SystemCursor::Default);
        }
        else
        {
            PlatformApplication::Get()->SetSystemCursor(cursorStack.Last());
        }
    }

    int FusionApplication::LoadImageResource(const IO::Path& resourcePath, const Name& imageName)
    {
        if (resourcePath.IsEmpty())
            return -1;
        
        if (resourcePath.Exists() && !resourcePath.IsDirectory())
        {
            CMImage image = CMImage::LoadFromFile(resourcePath);
            if (image.IsValid())
            {
                RPI::Texture* texture = new RPI::Texture(image);
                image.Free();

                if (texture->GetRhiTexture() != nullptr)
                {
                    loadedTextures.Insert(texture);
                    return RegisterImage(imageName, texture->GetRhiTexture());
                }
            }
        }

        return -1;
    }

    int FusionApplication::RegisterImage(const Name& imageName, RHI::Texture* image)
    {
        ZoneScoped;

        if (image == nullptr || !imageName.IsValid())
            return -1;
        if (registeredImagesByName.KeyExists(imageName))
            return registeredImagesByName[imageName];

        int index = registeredImages.GetCount();
        registeredImagesByName[imageName] = index;
        registeredImageIndices[image] = index;
        registeredImageNames[image] = imageName;

        registeredImages.Insert(image);

        imageRegistryUpdated = true;

        return index;
    }

    void FusionApplication::DeregisterImage(const Name& imageName)
    {
        if (registeredImagesByName.KeyExists(imageName))
        {
            int index = registeredImagesByName[imageName];
            registeredImageIndices.Remove(registeredImages[index]);
            registeredImages[index] = nullptr;
            registeredImagesByName.Remove(imageName);
        }

        imageRegistryUpdated = true;
    }

    void FusionApplication::DeregisterImage(RHI::Texture* image)
    {
        if (registeredImageIndices.KeyExists(image))
        {
            int index = registeredImageIndices[image];
            Name imageName = registeredImageNames[image];

            registeredImagesByName.Remove(imageName);
            registeredImageIndices.Remove(image);
            registeredImageNames.Remove(image);
            registeredImages[index] = nullptr;
        }
        imageRegistryUpdated = true;
    }

    RHI::Texture* FusionApplication::FindImage(const Name& imageName)
    {
        if (!registeredImagesByName.KeyExists(imageName))
            return nullptr;
        int index = registeredImagesByName[imageName];
        if (index >= registeredImages.GetCount() || index < 0)
            return nullptr;
        return registeredImages[index];
    }

    int FusionApplication::FindImageIndex(const Name& imageName)
    {
        if (!registeredImagesByName.KeyExists(imageName))
            return -1;
        return registeredImagesByName[imageName];
    }

    int FusionApplication::FindOrCreateSampler(const RHI::SamplerDescriptor& samplerDesc)
    {
        ZoneScoped;

        RHI::Sampler* sampler = RPISystem::Get().FindOrCreateSampler(samplerDesc);
        if (samplerIndices.KeyExists(sampler))
        {
            return samplerIndices[sampler];
        }

        int index = samplerArray.GetCount();
        samplerIndices[sampler] = index;
        samplerArray.Insert(sampler);
        samplersUpdated = true;
        return index;
    }

    void FusionApplication::Tick(bool isExposed)
    {
        ZoneScoped;

        this->isExposed = isExposed;

        for (int i = destructionQueue.GetSize() - 1; i >= 0; --i)
        {
            if (destructionQueue[i].frameCounter >= RHI::Limits::MaxSwapChainImageCount)
            {
                destructionQueue[i].object->Destroy();
                destructionQueue.RemoveAt(i);
                continue;
            }

            destructionQueue[i].frameCounter++;
        }

        for (int i = timers.GetSize() - 1; i >= 0; --i)
        {
            timers[i]->Tick();
        }

        int submittedFrameIdx = -1;

        if (rebuildFrameGraph)
        {
            rebuildFrameGraph = false;
            recompileFrameGraph = true;

            BuildFrameGraph();
            submittedFrameIdx = curImageIndex;
        }

        if (recompileFrameGraph)
        {
            recompileFrameGraph = false;

            CompileFrameGraph();
        }

        if (rootContext)
        {
            rootContext->Tick();
        }

        auto scheduler = FrameScheduler::Get();

        int imageIndex = scheduler->BeginExecution();

        if (imageIndex >= RHI::Limits::MaxSwapChainImageCount)
        {
            rebuildFrameGraph = recompileFrameGraph = true;
            return;
        }

        curImageIndex = imageIndex;
        
        RPISystem::Get().SimulationTick(curImageIndex);

        PrepareDrawList();

        scheduler->EndExecution();
    }

    void FusionApplication::BuildFrameGraph()
    {
        ZoneScoped;

        FrameScheduler* scheduler = FrameScheduler::Get();

        rebuildFrameGraph = false;
        recompileFrameGraph = true;

        scheduler->BeginFrameGraph();
        {
            rootContext->EmplaceFrameAttachments();

            rootContext->EnqueueScopes();
        }
        scheduler->EndFrameGraph();
    }

    void FusionApplication::CompileFrameGraph()
    {
        ZoneScoped;

        FrameScheduler* scheduler = FrameScheduler::Get();

        recompileFrameGraph = false;

        scheduler->Compile();
    }

    void FusionApplication::PrepareDrawList()
    {
        drawList.Shutdown();

        if (!rootContext)
            return;

        RHI::DrawListMask drawListMask{};
        HashSet<RHI::DrawListTag> drawListTags{};

        rootContext->SetDrawListMask(drawListMask);

        for (int i = 0; i < drawListMask.GetSize(); ++i)
        {
            if (drawListMask.Test(i))
            {
                drawListTags.Add((u8)i);
            }
        }

        drawList.Init(drawListMask);
        {
            rootContext->EnqueueDrawPackets(drawList, curImageIndex);
        }
        drawList.Finalize();

        fontManager->Flush(curImageIndex);

        if (imageRegistryUpdated)
        {
            textureSrg->Bind("_Textures", registeredImages.GetCount(), registeredImages.GetData());
            imageRegistryUpdated = false;
        }

        if (samplersUpdated)
        {
            textureSrg->Bind("_TextureSamplers", samplerArray.GetCount(), samplerArray.GetData());
            samplersUpdated = false;
        }

        textureSrg->FlushBindings();
        
        rootContext->SetDrawPackets(drawList);
    }

    void FusionApplication::RebuildFrameGraph()
    {
        rebuildFrameGraph = recompileFrameGraph = true;
    }

    // - Application Callbacks -

    void FusionApplication::QueueDestroy(Object* object)
    {
        destructionQueue.Add({ .object = object, .frameCounter = 0 });
    }

    u64 FusionApplication::ComputeMemoryFootprint()
    {
        u64 memory = Super::ComputeMemoryFootprint();

        memory += sizeof(RPI::Shader);
        memory += registeredImages.GetCapacity() * sizeof(void*);
        memory += loadedTextures.GetCapacity() * sizeof(void*);

        return memory;
    }

    void FusionApplication::OnWindowRestored(PlatformWindow* window)
    {
        RebuildFrameGraph();
    }

    void FusionApplication::OnWindowDestroyed(PlatformWindow* window)
    {
        RebuildFrameGraph();
        rootContext->MarkLayoutDirty();

        for (int i = rootContext->childContexts.GetSize() - 1; i >= 0; i--)
        {
            FFusionContext* context = rootContext->childContexts[i];

            if (!context->IsOfType<FNativeContext>())
                continue;

            FNativeContext* nativeContext = static_cast<FNativeContext*>(context);
            
            if (window->IsMainWindow() || nativeContext->GetPlatformWindow() == window)
            {
                if (nativeContext->owningWidget)
                    nativeContext->owningWidget->Destroy();
                nativeContext->owningWidget = nullptr;

                if (!nativeContext->isDestroyed)
                {
                    nativeContext->platformWindow = nullptr;
                    nativeContext->QueueDestroy();
                }
            }
        }
    }

    void FusionApplication::OnWindowClosed(PlatformWindow* window)
    {
        RebuildFrameGraph();
    }

    void FusionApplication::OnWindowResized(PlatformWindow* window, u32 newWidth, u32 newHeight)
    {
        RebuildFrameGraph();
        rootContext->MarkLayoutDirty();
    }

    void FusionApplication::OnWindowMinimized(PlatformWindow* window)
    {
        RebuildFrameGraph();
    }

    void FusionApplication::OnWindowCreated(PlatformWindow* window)
    {
        RebuildFrameGraph();
    }

    void FusionApplication::OnWindowExposed(PlatformWindow* window)
    {
        RebuildFrameGraph();
        rootContext->MarkLayoutDirty();
    }

    // - Shader Resources -

    void FusionApplication::InitializeShaders()
    {
        RawData vertexShader = GetFusionShaderVert();
        RawData fragmentShader = GetFusionShaderFrag();

        String vertexShaderJson = (char*)GetFusionShaderVertJson().data;
        String fragmentShaderJson = (char*)GetFusionShaderFragJson().data;

        JValue vertexReflection{};
        JValue fragmentReflection{};

        JsonSerializer::Deserialize2(vertexShaderJson, vertexReflection);
        JsonSerializer::Deserialize2(fragmentShaderJson, fragmentReflection);

        RPI::ShaderVariantDescriptor2 variantDesc{};
        variantDesc.shaderName = "FusionShader";
        variantDesc.entryPoints.Resize(2);
        variantDesc.entryPoints[0] = "VertMain";
        variantDesc.entryPoints[1] = "FragMain";

        variantDesc.moduleDesc.Resize(2);
        variantDesc.moduleDesc[0].byteCode = vertexShader.data;
        variantDesc.moduleDesc[0].byteSize = vertexShader.dataSize;
        variantDesc.moduleDesc[0].stage = ShaderStage::Vertex;
        variantDesc.moduleDesc[0].name = "VertMain";

        variantDesc.moduleDesc[1].byteCode = fragmentShader.data;
        variantDesc.moduleDesc[1].byteSize = fragmentShader.dataSize;
        variantDesc.moduleDesc[1].stage = ShaderStage::Fragment;
        variantDesc.moduleDesc[1].name = "FragMain";

        RHI::SRGVariableDescriptor perViewData{};
        perViewData.name = "_PerViewData";
        perViewData.bindingSlot = (u32)vertexReflection["ubos"][0]["binding"].GetNumberValue();
        perViewData.shaderStages = ShaderStage::Vertex | ShaderStage::Fragment;
        perViewData.type = ShaderResourceType::ConstantBuffer;

        variantDesc.reflectionInfo.FindOrAdd(SRGType::PerView)
            .TryAdd(perViewData);

        perViewSrgLayout = variantDesc.reflectionInfo.FindOrAdd(SRGType::PerView);

        {
            variantDesc.reflectionInfo.FindOrAdd(SRGType::PerDraw)
                .TryAdd(SRGVariableDescriptor(
					"_DrawList",
                    (u32)vertexReflection["ssbos"][0]["binding"].GetNumberValue(),
                    ShaderResourceType::StructuredBuffer,
                    ShaderStage::Vertex | ShaderStage::Fragment
                ))
        		.TryAdd(SRGVariableDescriptor(
                    "_ClipItems", 
                    (u32)vertexReflection["ssbos"][1]["binding"].GetNumberValue(),
                    ShaderResourceType::StructuredBuffer,
                    ShaderStage::Vertex | ShaderStage::Fragment
                ))
        		.TryAdd(SRGVariableDescriptor(
                    "_ShapeDrawList",
                    (u32)vertexReflection["ssbos"][2]["binding"].GetNumberValue(),
                    ShaderResourceType::StructuredBuffer,
                    ShaderStage::Vertex | ShaderStage::Fragment
                ))
        		.TryAdd(SRGVariableDescriptor(
                    "_LineItems",
                    (u32)vertexReflection["ssbos"][3]["binding"].GetNumberValue(),
                    ShaderResourceType::StructuredBuffer,
                    ShaderStage::Vertex | ShaderStage::Fragment
                ));

            variantDesc.reflectionInfo.FindOrAdd(SRGType::PerMaterial)
                .TryAdd(SRGVariableDescriptor(
                    "_FontAtlas",
                    (u32)fragmentReflection["separate_images"][0]["binding"].GetNumberValue(),
                    ShaderResourceType::Texture2D,
                    ShaderStage::Fragment
                ))
                .TryAdd(SRGVariableDescriptor(
                    "_FontAtlasSampler",
                    (u32)fragmentReflection["separate_samplers"][0]["binding"].GetNumberValue(),
                    ShaderResourceType::SamplerState,
                    ShaderStage::Fragment
                ))
                .TryAdd(SRGVariableDescriptor(
                    "_GlyphItems",
                    (u32)fragmentReflection["ssbos"][4]["binding"].GetNumberValue(),
                    ShaderResourceType::StructuredBuffer,
                    ShaderStage::Fragment
                ));

            variantDesc.reflectionInfo.FindOrAdd(SRGType::PerObject)
                .TryAdd(SRGVariableDescriptor(
                    "_Textures",
                    (u32)fragmentReflection["separate_images"][1]["binding"].GetNumberValue(),
                    ShaderResourceType::Texture2D,
                    ShaderStage::Fragment,
                    (u32)fragmentReflection["separate_images"][1]["array"][0].GetNumberValue()
                ))
        		.TryAdd(SRGVariableDescriptor(
                    "_TextureSamplers",
                    (u32)fragmentReflection["separate_samplers"][1]["binding"].GetNumberValue(),
                    ShaderResourceType::SamplerState,
                    ShaderStage::Fragment,
                    (u32)fragmentReflection["separate_samplers"][1]["array"][0].GetNumberValue()
                ));
        }

        perDrawSrgLayout = variantDesc.reflectionInfo.FindOrAdd(SRGType::PerDraw);
        perObjectSrgLayout = variantDesc.reflectionInfo.FindOrAdd(SRGType::PerObject);

        textureSrg = gDynamicRHI->CreateShaderResourceGroup(perObjectSrgLayout);

        Array<SamplerDescriptor> samplers = {
            { .addressModeU = SamplerAddressMode::ClampToBorder, .addressModeV = SamplerAddressMode::ClampToBorder, .samplerFilterMode = FilterMode::Linear, .borderColor = SamplerBorderColor::FloatTransparentBlack },
            { .addressModeU = SamplerAddressMode::Repeat, .addressModeV = SamplerAddressMode::ClampToBorder, .samplerFilterMode = FilterMode::Linear, .borderColor = SamplerBorderColor::FloatTransparentBlack  },
            { .addressModeU = SamplerAddressMode::ClampToBorder, .addressModeV = SamplerAddressMode::Repeat, .samplerFilterMode = FilterMode::Linear, .borderColor = SamplerBorderColor::FloatTransparentBlack  },
            { .addressModeU = SamplerAddressMode::Repeat, .addressModeV = SamplerAddressMode::Repeat, .samplerFilterMode = FilterMode::Linear, .borderColor = SamplerBorderColor::FloatTransparentBlack  }
        };

        for (const RHI::SamplerDescriptor& samplerDesc : samplers)
        {
            FindOrCreateSampler(samplerDesc);
        }

        textureSrg->Bind("_TextureSamplers", samplerArray.GetCount(), samplerArray.GetData());

        variantDesc.reflectionInfo.vertexInputs.Add("POSITION");
        variantDesc.reflectionInfo.vertexInputs.Add("TEXCOORD0");

        fusionShader = new RPI::Shader();
        fusionShader->AddVariant(variantDesc);
    }

} // namespace CE

