#include "FusionCore.h"

namespace CE
{
    static FusionApplication* gInstance = nullptr;

    extern RawData GetFusionShaderVert();
    extern RawData GetFusionShaderFrag();
    extern RawData GetFusionShaderVertJson();
    extern RawData GetFusionShaderFragJson();
    extern RawData GetRobotoFont();

    FusionApplication::FusionApplication()
    {
        fontManager = CreateDefaultSubobject<FFontManager>("FontManager");
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

    void FusionApplication::Tick(bool isExposed)
    {
        ZoneScoped;

        this->isExposed = isExposed;

        for (int i = destructionQueue.GetSize() - 1; i >= 0; --i)
        {
	        if (destructionQueue[i].frameCounter > RHI::Limits::MaxSwapChainImageCount)
	        {
                destructionQueue[i].object->Destroy();
                destructionQueue.RemoveAt(i);
		        continue;
	        }

            destructionQueue[i].frameCounter++;
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

        FrameAttachmentDatabase& attachmentDatabase = scheduler->GetAttachmentDatabase();

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

        rootContext->SetDrawPackets(drawList);
    }

    void FusionApplication::SetRootContext(FFusionContext* context)
    {
        rootContext = context;
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
                    delete nativeContext;
                }

                rootContext->childContexts.RemoveAt(i);
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
                    (u32)fragmentReflection["ssbos"][3]["binding"].GetNumberValue(),
                    ShaderResourceType::StructuredBuffer,
                    ShaderStage::Fragment
                ));
        }

        perDrawSrgLayout = variantDesc.reflectionInfo.FindOrAdd(SRGType::PerDraw);

        variantDesc.reflectionInfo.vertexInputs.Add("POSITION");
        variantDesc.reflectionInfo.vertexInputs.Add("TEXCOORD0");

        fusionShader = new RPI::Shader();
        fusionShader->AddVariant(variantDesc);
    }

} // namespace CE

