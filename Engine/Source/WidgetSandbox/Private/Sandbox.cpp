#include "WidgetSandbox.h"

static const CE::String css = R"(
.Class1 .Class2::tab:hover { /* some comment */
	padding: 0% 0px;
	icon: url('path/to/icon');
	icon: url("path/to/icon");
}

CButton:hover, CLabel.SomeClass, #SomeName, CWindow > CToolBar, CTextInput[mode="password"], * {
	background: rgba(255, 255, 255, 255);
}
)";

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

		mainWindow = window;
		mainWindow->SetBorderless(true);

		RHI::SwapChainDescriptor swapChainDesc{};
		swapChainDesc.imageCount = 2;
		swapChainDesc.preferredFormats = { RHI::Format::R8G8B8A8_UNORM, RHI::Format::B8G8R8A8_UNORM };

		swapChain = RHI::gDynamicRHI->CreateSwapChain(mainWindow, swapChainDesc);

		mainWindow->GetDrawableWindowSize(&width, &height);

		PlatformApplication::Get()->AddMessageHandler(this);

		CApplication* app = CApplication::Get();

		app->LoadGlobalStyleSheet(PlatformDirectories::GetLaunchDir() / "Engine/Styles/Style.css");

		mainDockSpace = CreateWindow<CDockSpace>(MODULE_NAME, nullptr, mainWindow);
		mainDockWindow = CreateWindow<CDockWindow>(MODULE_NAME, mainDockSpace);
		//auto secondWindow = CreateWindow<CDockWindow>("Second", mainDockSpace);

		widgetWindows.Add(mainDockSpace);
		//widgetWindows.Add(mainDockWindow);
		//widgetWindows.Add(secondWindow);
		
		InitFontAtlas();
		InitWidgets();

		BuildFrameGraph();
		CompileFrameGraph();

		f32 timeTaken = ((f32)(clock() - prevTime)) / CLOCKS_PER_SEC;

		CE_LOG(Info, All, "Initialization time: {} seconds", timeTaken);
	}

	void WidgetSandbox::Tick(f32 deltaTime)
	{
		if (destroyed)
			return;

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

		delete swapChain; swapChain = nullptr;
		delete swapChain2; swapChain2 = nullptr;

		rpiSystem.Shutdown();
	}
	
	void WidgetSandbox::InitFontAtlas()
	{
		AssetManager* assetManager = gEngine->GetAssetManager();

		auto prevTime = clock();

		auto timeTaken = ((f32)(clock() - prevTime)) / CLOCKS_PER_SEC;
	}

	void WidgetSandbox::InitWidgets()
	{
		CWidget* toolBar = CreateObject<CWidget>(mainDockWindow, "ToolBar");

		for (int i = 0; i < 4; ++i)
		{
			CWidget* btn = CreateObject<CWidget>(toolBar, "Button");
		}
	}

	void WidgetSandbox::DestroyWidgets()
	{
	}

	void WidgetSandbox::BuildFrameGraph()
	{
		rebuild = false;
		recompile = true;

		u32 screenWidth = swapChain->GetWidth();
		u32 screenHeight = swapChain->GetHeight();

		scheduler->BeginFrameGraph();
		{
			RHI::FrameAttachmentDatabase& attachmentDatabase = scheduler->GetFrameAttachmentDatabase();

			attachmentDatabase.EmplaceFrameAttachment(mainWindow->GetTitle(), swapChain);

			if (!mainWindow->IsMinimized() && mainWindow->IsShown())
			{
				scheduler->BeginScope(mainWindow->GetTitle());
				{
					RHI::ImageScopeAttachmentDescriptor swapChainAttachment{};
					swapChainAttachment.attachmentId = mainWindow->GetTitle();
					swapChainAttachment.loadStoreAction.clearValue = Vec4(0.15f, 0.15f, 0.15f, 1);
					swapChainAttachment.loadStoreAction.loadAction = RHI::AttachmentLoadAction::Clear;
					swapChainAttachment.loadStoreAction.storeAction = RHI::AttachmentStoreAction::Store;
					swapChainAttachment.multisampleState.sampleCount = 1;
					scheduler->UseAttachment(swapChainAttachment, RHI::ScopeAttachmentUsage::Color, RHI::ScopeAttachmentAccess::Write);

					scheduler->PresentSwapChain(swapChain);
				}
				scheduler->EndScope();
			}
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
		drawListMask.Set(mainDockSpace->GetDrawListTag());
		drawList.Init(drawListMask);
		
		// Add items
		for (int i = 0; i < widgetWindows.GetSize(); i++)
		{
			if (!widgetWindows[i]->IsVisible() || !widgetWindows[i]->IsEnabled())
				continue;

			const Array<RHI::DrawPacket*>& drawPackets = widgetWindows[i]->FlushDrawPackets(imageIndex);
			for (RHI::DrawPacket* drawPacket : drawPackets)
			{
				drawList.AddDrawPacket(drawPacket);
			}
		}

		auto prevTime = clock();

		static bool firstTime = true;

		// Finalize
		drawList.Finalize();

		scheduler->SetScopeDrawList(mainWindow->GetTitle(), &drawList.GetDrawListForTag(mainDockSpace->GetDrawListTag()));
	}

	void WidgetSandbox::OnWindowResized(PlatformWindow* window, u32 newWidth, u32 newHeight)
	{
		rebuild = recompile = true;

		scheduler->WaitUntilIdle();

		if (!mainWindow->IsMinimized())
			swapChain->Rebuild();
	}

	void WidgetSandbox::OnWindowDestroyed(PlatformWindow* window)
	{
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

} // namespace CE
