#include "WidgetSandbox.h"

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

		RHI::SwapChainDescriptor swapChainDesc{};
		swapChainDesc.imageCount = 2;
		swapChainDesc.preferredFormats = { RHI::Format::R8G8B8A8_UNORM, RHI::Format::B8G8R8A8_UNORM };
#if FORCE_SRGB
		swapChainDesc.preferredFormats = { RHI::Format::R8G8B8A8_SRGB, RHI::Format::B8G8R8A8_SRGB };
#endif

		swapChain = RHI::gDynamicRHI->CreateSwapChain(mainWindow, swapChainDesc);

		swapChain2 = RHI::gDynamicRHI->CreateSwapChain(secondWindow, swapChainDesc);

		mainWindow->GetDrawableWindowSize(&width, &height);

		PlatformApplication::Get()->AddMessageHandler(this);

		CApplication* app = CApplication::Get();
		widgetWindows.Add(app->CreateWindow(MODULE_NAME, MODULE_NAME, mainWindow));
		widgetWindows.Add(app->CreateWindow("Second", "Second", secondWindow));
		windowMain = widgetWindows[0];
		windowSecond = widgetWindows[1];
		
		InitFontAtlas();

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

		u32 curWidth2 = 0, curHeight2 = 0;
		secondWindow->GetDrawableWindowSize(&curWidth2, &curHeight2);

		if (InputManager::IsKeyDown(KeyCode::Backspace))
		{
			if (secondWindowHidden)
				secondWindow->Show();
			else
				secondWindow->Hide();

			secondWindowHidden = !secondWindowHidden;
		}

		if (width != curWidth || height != curHeight)
		{
			rebuild = recompile = true;
			width = curWidth;
			height = curHeight;
		}

		if (width2 != curWidth2 || height2 != curHeight2)
		{
			rebuild = recompile = true;
			width2 = curWidth2;
			height2 = curHeight2;
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
	}

	void WidgetSandbox::Shutdown()
	{
		delete scheduler;

		for (CWindow* window : widgetWindows)
		{
			window->Destroy();
		}
		widgetWindows.Clear();

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

			attachmentDatabase.EmplaceFrameAttachment("SwapChain", swapChain);
			attachmentDatabase.EmplaceFrameAttachment("SwapChain2", swapChain2);

			if (!mainWindow->IsMinimized() && mainWindow->IsShown())
			{
				scheduler->BeginScope(windowMain->GetName());
				{
					RHI::ImageScopeAttachmentDescriptor swapChainAttachment{};
					swapChainAttachment.attachmentId = "SwapChain";
					swapChainAttachment.loadStoreAction.clearValue = Vec4(0.15f, 0.15f, 0.15f, 1);
					swapChainAttachment.loadStoreAction.loadAction = RHI::AttachmentLoadAction::Clear;
					swapChainAttachment.loadStoreAction.storeAction = RHI::AttachmentStoreAction::Store;
					swapChainAttachment.multisampleState.sampleCount = 1;
					scheduler->UseAttachment(swapChainAttachment, RHI::ScopeAttachmentUsage::Color, RHI::ScopeAttachmentAccess::Write);

					scheduler->PresentSwapChain(swapChain);
				}
				scheduler->EndScope();
			}

			if (!secondWindow->IsMinimized() && secondWindow->IsShown())
			{
				scheduler->BeginScope(windowSecond->GetName());
				{
					RHI::ImageScopeAttachmentDescriptor swapChainAttachment{};
					swapChainAttachment.attachmentId = "SwapChain2";
					swapChainAttachment.loadStoreAction.clearValue = Vec4(0, 0.5f, 0, 1);
					swapChainAttachment.loadStoreAction.loadAction = RHI::AttachmentLoadAction::Clear;
					swapChainAttachment.loadStoreAction.storeAction = RHI::AttachmentStoreAction::Store;
					swapChainAttachment.multisampleState.sampleCount = 1;
					scheduler->UseAttachment(swapChainAttachment, RHI::ScopeAttachmentUsage::Color, RHI::ScopeAttachmentAccess::Write);

					scheduler->PresentSwapChain(swapChain2);
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
		for (CWindow* window : widgetWindows)
		{
			drawListMask.Set(window->GetDrawListTag());
		}
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

		scheduler->SetScopeDrawList(windowMain->GetName(), &drawList.GetDrawListForTag(widgetWindows[0]->GetDrawListTag()));
		scheduler->SetScopeDrawList(windowSecond->GetName(), &drawList.GetDrawListForTag(widgetWindows[1]->GetDrawListTag()));
	}

	void WidgetSandbox::OnWindowResized(PlatformWindow* window, u32 newWidth, u32 newHeight)
	{
		rebuild = recompile = true;

		scheduler->WaitUntilIdle();

		if (!mainWindow->IsMinimized())
			swapChain->Rebuild();
		if (!secondWindow->IsMinimized())
			swapChain2->Rebuild();
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
