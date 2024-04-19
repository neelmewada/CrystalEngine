#include "System.h"

namespace CE
{
	RHI::Buffer* vertexBuffer = nullptr;

    RendererSubsystem::RendererSubsystem()
    {
		
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

			CApplicationInitInfo appInitInfo{};
			appInitInfo.draw2dShader = renderer2dShader->GetOrCreateRPIShader(0);
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

		CApplication* app = CApplication::TryGet();

		RPISystem::Get().Shutdown();

		if (app)
		{
			PlatformApplication::Get()->RemoveMessageHandler(this);

			app->Shutdown();
			app->Destroy();
		}

		delete scheduler; scheduler = nullptr;
	}

	void RendererSubsystem::Shutdown()
	{
		Super::Shutdown();

		
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

		u32 imageIndex = scheduler->BeginExecution();

		if (imageIndex >= RHI::Limits::MaxSwapChainImageCount)
		{
			rebuildFrameGraph = recompileFrameGraph = true;
			return;
		}

		SubmitDrawPackets(imageIndex);

		scheduler->EndExecution();
	}

	void RendererSubsystem::BuildFrameGraph()
	{
		rebuildFrameGraph = false;
		recompileFrameGraph = true;

		scheduler->BeginFrameGraph();
		{
			auto app = CApplication::TryGet();

			// TODO: Build render pipeline passes

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

} // namespace CE
