#include "WidgetSandbox.h"

using namespace CE::Widgets;

namespace CE
{
	static int counter = 0;
	static RPI::RPISystem& rpiSystem = RPI::RPISystem::Get();

	void WidgetSandbox::Init(PlatformWindow* window)
	{
		auto prevTime = clock();

		localCounter = counter++;
		rpiSystem.Initialize();
		
		RHI::FrameSchedulerDescriptor desc{};
		desc.numFramesInFlight = 2;
		
		scheduler = RHI::FrameScheduler::Create(desc);

		auto assetManager = gEngine->GetAssetManager();

		auto renderer2dShader = assetManager->LoadAssetAtPath<CE::Shader>("/Engine/Assets/Shaders/2D/SDFGeometry");

		auto fontAsset = assetManager->LoadAssetAtPath<Font>("/Engine/Assets/Fonts/Roboto");
		auto poppinsFont = assetManager->LoadAssetAtPath<Font>("/Engine/Assets/Fonts/Poppins");

		auto atlasData = fontAsset->GetAtlasData();

		CApplicationInitInfo appInitInfo{};
		appInitInfo.draw2dShader = renderer2dShader->GetOrCreateRPIShader(0);
		appInitInfo.defaultFont = atlasData;
		appInitInfo.defaultFontName = "Roboto";
		appInitInfo.numFramesInFlight = 2;
		appInitInfo.scheduler = scheduler;

		CApplication::Get()->Initialize(appInitInfo);

		CApplication::Get()->RegisterFont("Poppins", poppinsFont->GetAtlasData());

		mainWindow = window;
		mainWindow->SetBorderless(true);
		platformWindows.Add(mainWindow);

		//RHI::SwapChainDescriptor swapChainDesc{};
		//swapChainDesc.imageCount = 2;
		//swapChainDesc.preferredFormats = { RHI::Format::R8G8B8A8_UNORM, RHI::Format::B8G8R8A8_UNORM };

		//swapChain = RHI::gDynamicRHI->CreateSwapChain(mainWindow, swapChainDesc);

		mainWindow->SetMinimumSize(Vec2i(1280, 720));
		mainWindow->GetDrawableWindowSize(&width, &height);

		PlatformApplication::Get()->AddMessageHandler(this);

		CApplication* app = CApplication::Get();

		app->LoadGlobalStyleSheet(PlatformDirectories::GetLaunchDir() / "Engine/Styles/Style.css");
		
		InitFontAtlas();
		InitWidgets();

		BuildFrameGraph();
		CompileFrameGraph();

		f32 timeTaken = ((f32)(clock() - prevTime)) / CLOCKS_PER_SEC;

		CE_LOG(Info, All, "Initialization time: {} seconds", timeTaken);
	}

	void WidgetSandbox::InitWidgets()
	{
		mainDockSpace = CreateWindow<CDockSpace>(MODULE_NAME, nullptr, mainWindow);
		mainDockWindow = CreateWindow<CDockWindow>(MODULE_NAME, mainDockSpace->GetRootDockSplit());
		mainDockWindow->SetAsMainWindow(true);

		CWidget* toolBar = CreateObject<CWidget>(mainDockWindow, "ToolBar");

		for (int i = 0; i < 4; ++i)
		{
			CWidget* btn = CreateObject<CWidget>(toolBar, "TestButton");
		}
		
		minorDockSpace = CreateWindow<CDockSpace>("MinorDockSpace", nullptr);
		minorDockSpace->SetDockType(CDockType::Minor);
		mainDockWindow->AddSubWidget(minorDockSpace);

		minorDockSpace->Split(0.25f, CDockSplitDirection::Horizontal, "SplitLeft", "SplitRight");
		auto parentSplit = minorDockSpace->GetRootDockSplit();
		auto left = parentSplit->GetSplit(0);
		auto right = parentSplit->GetSplit(1);

		minorDockSpace->Split(right, 0.5f, CDockSplitDirection::Vertical, "SplitRightTop", "SplitRightBottom");
		auto rightTop = right->GetSplit(0);
		auto rightBottom = right->GetSplit(1);

		secondDockWindow = CreateWindow<CDockWindow>("Second", left);
		thirdDockWindow = CreateWindow<CDockWindow>("Third", rightTop);
		fourthDockWindow = CreateWindow<CDockWindow>("Fourth", rightBottom);
		fifthDockWindow = CreateWindow<CDockWindow>("Fifth", rightBottom);

		for (int i = 0; i < 24; ++i)
		{
			CButton* btn = CreateObject<CButton>(thirdDockWindow, "Button");
			btn->SetText(String("Button ") + i);
		}

		if (false)
		{
			CButton* btn = CreateObject<CButton>(fourthDockWindow, "Button");
			btn->SetText("Click On this Button link");

			Object::Bind(btn, MEMBER_FUNCTION(CButton, OnButtonLeftClicked), [btn]
				{
					CE_LOG(Info, All, "Button Clicked!");
				});
		}
	}

	void WidgetSandbox::DestroyWidgets()
	{
	}

	void WidgetSandbox::Tick(f32 deltaTime)
	{
		if (destroyed)
			return;

		if (InputManager::IsKeyDown(KeyCode::Space))
		{
			for (CButton* button : buttons)
			{
				button->SetEnabled(true);
			}
		}

		u32 curWidth = 0, curHeight = 0;
		mainWindow->GetDrawableWindowSize(&curWidth, &curHeight);

		if (width != curWidth || height != curHeight)
		{
			rebuild = recompile = true;
			width = curWidth;
			height = curHeight;
		}

		if (rebuild)
		{
			rebuild = false;
			recompile = true;

			BuildFrameGraph();
		}

		if (recompile)
		{
			recompile = false;
			resubmit = true;

			CompileFrameGraph();
		}

		u32 imageIndex = scheduler->BeginExecution();

		if (imageIndex >= RHI::Limits::MaxSwapChainImageCount)
		{
			rebuild = recompile = true;
			return;
		}

		SubmitWork(imageIndex);

		scheduler->EndExecution();

		if (InputManager::IsKeyDown(KeyCode::Escape))
		{
			RequestEngineExit("USER_EXIT");
		}
	}

	void WidgetSandbox::Shutdown()
	{
		DestroyWidgets();

		delete scheduler;

		PlatformApplication::Get()->RemoveMessageHandler(this);

		rpiSystem.Shutdown();
	}
	
	void WidgetSandbox::InitFontAtlas()
	{
		AssetManager* assetManager = gEngine->GetAssetManager();

		auto prevTime = clock();

		auto timeTaken = ((f32)(clock() - prevTime)) / CLOCKS_PER_SEC;
	}

	void WidgetSandbox::BuildFrameGraph()
	{
		rebuild = false;
		recompile = true;

		scheduler->BeginFrameGraph();
		{
			CApplication::Get()->BuildFrameGraph();
		}
		scheduler->EndFrameGraph();
	}

	void WidgetSandbox::CompileFrameGraph()
	{
		recompile = false;
		scheduler->Compile();

		RHI::TransientMemoryPool* pool = scheduler->GetTransientPool();
		RHI::MemoryHeap* imageHeap = pool->GetImagePool();
		if (imageHeap != nullptr)
		{
			CE_LOG(Info, All, "Transient Image Pool: {} MB", (imageHeap->GetHeapSize() / 1024.0f / 1024.0f));
		}
	}

	void WidgetSandbox::SubmitWork(u32 imageIndex)
	{
		resubmit = false;
		drawList.Shutdown();

		RHI::DrawListMask drawListMask{};
		//drawListMask.Set(mainDockSpace->GetDrawListTag());
		CApplication::Get()->SetDrawListMasks(drawListMask);
		drawList.Init(drawListMask);

		CApplication::Get()->FlushDrawPackets(drawList, imageIndex);

		auto prevTime = clock();

		static bool firstTime = true;

		// Finalize
		drawList.Finalize();

		CApplication::Get()->SubmitDrawPackets(drawList);
		//scheduler->SetScopeDrawList(mainWindow->GetTitle(), &drawList.GetDrawListForTag(mainDockSpace->GetDrawListTag()));
	}

	void WidgetSandbox::OnWindowResized(PlatformWindow* window, u32 newWidth, u32 newHeight)
	{
		rebuild = recompile = true;

		scheduler->WaitUntilIdle();

		//if (!mainWindow->IsMinimized())
		//	swapChain->Rebuild();
	}

	void WidgetSandbox::OnWindowDestroyed(PlatformWindow* window)
	{
		rebuild = recompile = true;

		if (window == this->mainWindow)
		{
			destroyed = true;
			this->mainWindow = nullptr;
		}
	}

	void WidgetSandbox::OnWindowMinimized(PlatformWindow* window)
	{
		rebuild = recompile = true;
	}

	void WidgetSandbox::OnWindowRestored(PlatformWindow* window)
	{
		rebuild = recompile = true;
	}

	void WidgetSandbox::OnWindowCreated(PlatformWindow* window)
	{
		rebuild = recompile = true;
	}

} // namespace CE
