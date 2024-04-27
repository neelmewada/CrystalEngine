#include "System.h"

namespace CE
{
	RHI::Buffer* vertexBuffer = nullptr;

    RendererSubsystem::RendererSubsystem()
    {
		
    }

    void RendererSubsystem::RebuildFrameGraph()
    {
		rebuildFrameGraph = recompileFrameGraph = true;
    }

    f32 RendererSubsystem::GetTickPriority() const
    {
		return 2;
    }

    RPI::Texture* RendererSubsystem::LoadImage(const Name& assetPath)
    {
		AssetManager* assetManager = gEngine->GetAssetManager();
		CE::Texture* texture = assetManager->LoadAssetAtPath<CE::Texture>(assetPath);
		if (!texture)
			return nullptr;
		return texture->GetRpiTexture();
    }

    void RendererSubsystem::OnWindowCreated(PlatformWindow* window)
    {
		rebuildFrameGraph = recompileFrameGraph = true;
    }

    void RendererSubsystem::OnWindowDestroyed(PlatformWindow* window)
    {
		rebuildFrameGraph = recompileFrameGraph = true;
    }

    void RendererSubsystem::OnWindowClosed(PlatformWindow* window)
    {
		rebuildFrameGraph = recompileFrameGraph = true;
    }

    void RendererSubsystem::OnWindowMinimized(PlatformWindow* window)
    {
		rebuildFrameGraph = recompileFrameGraph = true;
    }

    void RendererSubsystem::OnWindowResized(PlatformWindow* window, u32 newWidth, u32 newHeight)
    {
		rebuildFrameGraph = recompileFrameGraph = true;
    }

    void RendererSubsystem::OnWindowRestored(PlatformWindow* window)
    {
		rebuildFrameGraph = recompileFrameGraph = true;
    }

    void RendererSubsystem::Initialize()
	{
		Super::Initialize();


	}

	void RendererSubsystem::PostInitialize()
	{
		Super::PostInitialize();

		sceneSubsystem = gEngine->GetSubsystem<SceneSubsystem>();

		RPISystem::Get().Initialize();

		// - Feature Processors -

		RegisterFeatureProcessor<StaticMeshComponent, StaticMeshFeatureProcessor>();

		// - Scheduler -

		RHI::FrameSchedulerDescriptor desc{};
		desc.numFramesInFlight = 2;

		scheduler = FrameScheduler::Create(desc);

		PlatformWindow* mainWindow = PlatformApplication::Get()->GetMainWindow();

		if (mainWindow)
		{
			PlatformApplication::Get()->AddMessageHandler(this);

			auto assetManager = gEngine->GetAssetManager();

			auto renderer2dShader = assetManager->LoadAssetAtPath<CE::Shader>("/Engine/Assets/Shaders/2D/SDFGeometry");

			auto fontAsset = assetManager->LoadAssetAtPath<Font>("/Engine/Assets/Fonts/Roboto");
			auto poppinsFont = assetManager->LoadAssetAtPath<Font>("/Engine/Assets/Fonts/Poppins");

			auto atlasData = fontAsset->GetAtlasData();

			RPI::ShaderCollection* draw2dShaderCollection = renderer2dShader->GetShaderCollection();
			
			CApplicationInitInfo appInitInfo{};
			appInitInfo.draw2dShader = draw2dShaderCollection->At(0).shader;
			appInitInfo.defaultFont = atlasData;
			appInitInfo.defaultFontName = "Roboto";
			appInitInfo.scheduler = GetScheduler();
			appInitInfo.resourceLoader = this;

			CApplication::Get()->Initialize(appInitInfo);

			CApplication::Get()->RegisterFont("Poppins", poppinsFont->GetAtlasData());
		}
	}

	void RendererSubsystem::PreShutdown()
	{
		Super::PreShutdown();

		delete scheduler; scheduler = nullptr;
	}

	void RendererSubsystem::Shutdown()
	{
		Super::Shutdown();

		CApplication* app = CApplication::TryGet();

		RPISystem::Get().Shutdown();

		if (app)
		{
			PlatformApplication::Get()->RemoveMessageHandler(this);

			app->Shutdown();
			app->Destroy();
		}
	}

	void RendererSubsystem::Tick(f32 delta)
	{
		Super::Tick(delta);

		CApplication* app = CApplication::TryGet();

		if (app)
		{
			app->Tick();
		}

		if (rebuildFrameGraph)
		{
			rebuildFrameGraph = false;
			recompileFrameGraph = true;

			BuildFrameGraph();
		}

		if (recompileFrameGraph)
		{
			recompileFrameGraph = false;

			CompileFrameGraph();
		}

		curImageIndex = scheduler->BeginExecution();

		if (curImageIndex >= RHI::Limits::MaxSwapChainImageCount)
		{
			rebuildFrameGraph = recompileFrameGraph = true;
			return;
		}

		CE::Scene* scene = sceneSubsystem->GetActiveScene();
		RPI::Scene* rpiScene = scene->GetRpiScene();

		RPISystem::Get().SimulationTick(curImageIndex);
		RPISystem::Get().RenderTick(curImageIndex);

		SubmitDrawPackets(curImageIndex);

		scheduler->EndExecution();
	}

	void RendererSubsystem::BuildFrameGraph()
	{
		rebuildFrameGraph = false;
		recompileFrameGraph = true;
    	
    	CE::Scene* scene = sceneSubsystem->GetActiveScene();
    	// TODO: Enqueue draw packets early! Scope producers need to have all draw packets available beforehand.
    	if (scene)
    	{
    		
    	}
    	
		scheduler->BeginFrameGraph();
		{
			auto app = CApplication::TryGet();
			
			if (scene)
			{
				for (CE::RenderPipeline* renderPipeline : scene->renderPipelines)
				{
					renderPipeline->GetRpiRenderPipeline()->ImportScopeProducers(scheduler);
				}
			}
			
			if (app)
			{
				app->BuildFrameGraph();
			}
		}
		scheduler->EndFrameGraph();
	}

	void RendererSubsystem::CompileFrameGraph()
	{
		recompileFrameGraph = false;

		scheduler->Compile();

		RHI::TransientMemoryPool* pool = scheduler->GetTransientPool();
		RHI::MemoryHeap* imageHeap = pool->GetImagePool();
		if (imageHeap != nullptr)
		{
			CE_LOG(Info, All, "Transient Image Pool: {} MB", (imageHeap->GetHeapSize() / 1024.0f / 1024.0f));
		}
	}

	void RendererSubsystem::SubmitDrawPackets(int imageIndex)
	{
		drawList.Shutdown();

		RHI::DrawListMask drawListMask{};

		CApplication* app = CApplication::TryGet();

		if (app)
		{
			CApplication::Get()->SetDrawListMasks(drawListMask);
		}

		drawList.Init(drawListMask);

		if (app)
		{
			app->FlushDrawPackets(drawList, imageIndex);
		}

		// Finalize
		drawList.Finalize();

		if (app)
		{
			app->SubmitDrawPackets(drawList);
		}
	}

	void RendererSubsystem::RegisterFeatureProcessor(SubClass<ActorComponent> componentClass,
		SubClass<FeatureProcessor> fpClass)
	{
		if (componentClass == nullptr || fpClass == nullptr)
			return;

		componentClassToFeatureProcessorClass[componentClass] = fpClass;
	}

	SubClass<FeatureProcessor> RendererSubsystem::GetFeatureProcessClass(SubClass<ActorComponent> componentClass)
	{
		if (componentClass == nullptr)
			return nullptr;

		Class* parentClass = componentClass;

		while (parentClass != nullptr)
		{
			if (parentClass->GetSuperClassCount() == 0 || parentClass == GetStaticClass<Object>())
				break;

			if (componentClassToFeatureProcessorClass.KeyExists(parentClass))
			{
				return componentClassToFeatureProcessorClass[parentClass];
			}
			parentClass = parentClass->GetSuperClass(0);
		}

		return nullptr;
	}

} // namespace CE
