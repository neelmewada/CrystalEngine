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

		mainWindow->AddListener(this);
		secondWindow->AddListener(this);
		
		//InitHDRIs();
		//InitCubeMapDemo();
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
		if (mainWindow)
		{
			mainWindow->RemoveListener(this);
		}

		if (secondWindow)
		{
			secondWindow->RemoveListener(this);
		}

		delete scheduler;

		delete renderer2d; renderer2d = nullptr;

		delete swapChain; swapChain = nullptr;
		delete swapChain2; swapChain2 = nullptr;

		rpiSystem.Shutdown();
	}
	
	void WidgetSandbox::InitFontAtlas()
	{
		AssetManager* assetManager = gEngine->GetAssetManager();

		auto prevTime = clock();

		renderer2dShader = assetManager->LoadAssetAtPath<CE::Shader>("/Engine/Assets/Shaders/2D/SDFGeometry");

		fontAsset = assetManager->LoadAssetAtPath<Font>("/Engine/Assets/Fonts/Roboto");

		atlasData = fontAsset->GetAtlasData();

		auto timeTaken = ((f32)(clock() - prevTime)) / CLOCKS_PER_SEC;

		UpdateTextData();
	}

	void WidgetSandbox::InitWidgets()
	{
	}

	void WidgetSandbox::DestroyWidgets()
	{
	}

	void WidgetSandbox::UpdateTextData()
	{
		static const String text = "The quick brown fox jumps over the lazy dog\nNew line. <>?![]{};'\"/\\=+-_*!@#$%^&()~`";

		u32 screenWidth = swapChain->GetWidth();
		u32 screenHeight = swapChain->GetHeight();

		if (renderer2d == nullptr) // First time setup
		{
			Renderer2DDescriptor desc{};
			desc.screenSize = Vec2i(screenWidth, screenHeight);
			//desc.textShader = textShader->GetOrCreateRPIShader(0);
			desc.drawShader = renderer2dShader->GetOrCreateRPIShader(0);
			desc.drawListTag = uiTag;
			desc.numFramesInFlight = swapChain->GetImageCount();
			desc.multisampling.sampleCount = 1;
			desc.multisampling.quality = 1.0f;
			
			desc.viewConstantData.viewMatrix = Matrix4x4::Identity();
			desc.viewConstantData.projectionMatrix = Matrix4x4::Translation(Vec3(-1, -1, 0)) * Matrix4x4::Scale(Vec3(1.0f / screenWidth, 1.0f / screenHeight, 1)) * Quat::EulerDegrees(0, 0, 0).ToMatrix();
			desc.viewConstantData.viewProjectionMatrix = desc.viewConstantData.projectionMatrix * desc.viewConstantData.viewMatrix;
			desc.viewConstantData.viewPosition = Vec4(0, 0, 0, 0);
			
			renderer2d = new Renderer2D(desc);

			renderer2d->RegisterFont("Roboto", atlasData);
		}
		else
		{
			
		}
	}

	void WidgetSandbox::BuildFrameGraph()
	{
		rebuild = false;
		recompile = true;

		RHI::MultisampleState msaa{};
		msaa.sampleCount = 1;
		msaa.quality = 1.0f;

		u32 screenWidth = swapChain->GetWidth();
		u32 screenHeight = swapChain->GetHeight();
		
		UpdateTextData();

		// Update renderer2D's view constants with new screen resolution
		{
			RPI::PerViewConstants viewConstantData{};
			viewConstantData.viewMatrix = Matrix4x4::Identity();
			viewConstantData.projectionMatrix = Matrix4x4::Translation(Vec3(-1, -1, 0)) * Matrix4x4::Scale(Vec3(1.0f / screenWidth, 1.0f / screenHeight, 1)) * Quat::EulerDegrees(0, 0, 0).ToMatrix();
			viewConstantData.viewProjectionMatrix = viewConstantData.projectionMatrix * viewConstantData.viewMatrix;
			viewConstantData.viewPosition = Vec4(0, 0, 0, 0);

			renderer2d->SetScreenSize(Vec2i(swapChain->GetWidth(), swapChain->GetHeight()));
			renderer2d->SetViewConstants(viewConstantData);
		}

		scheduler->BeginFrameGraph();
		{
			RHI::FrameAttachmentDatabase& attachmentDatabase = scheduler->GetFrameAttachmentDatabase();

			attachmentDatabase.EmplaceFrameAttachment("SwapChain", swapChain);
			attachmentDatabase.EmplaceFrameAttachment("SwapChain2", swapChain2);

			scheduler->BeginScope("UI");
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

			scheduler->BeginScope("UI2");
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
		drawListMask.Set(uiTag);
		//drawListMask.Set(transparentTag);
		drawList.Init(drawListMask);
		
		// Add items
		for (int i = 0; i < uiDrawPackets.GetSize(); i++)
		{
			drawList.AddDrawPacket(uiDrawPackets[i]);
		}

		auto prevTime = clock();

		renderer2d->Begin();
		{
			renderer2d->PushFont("Roboto", 16);

			renderer2d->SetFillColor(Color(0, 0, 0, 1));
			Vec2 pos = Vec2(0, 200);
			renderer2d->SetCursor(pos);

			renderer2d->SetRotation(0.0f);
			String text1 = "This is a line.\nThis is new line.";
			Vec2 size = renderer2d->CalculateTextSize(text1, 100);
			renderer2d->DrawRect(size);

			renderer2d->SetFillColor(Color(1, 1, 1, 1));
			renderer2d->DrawText(text1, size);

			renderer2d->SetCursor(pos + Vec2(0, size.y));
			renderer2d->SetFillColor(Color(1, 0, 0, 1));
			size = renderer2d->DrawText("This is separate text");

			renderer2d->SetRotation(15.0f);
			renderer2d->SetFillColor(Color::White());
			renderer2d->SetOutlineColor(Color::Blue());
			renderer2d->SetBorderThickness(5.0f);

			renderer2d->SetCursor(Vec2(200, 200));
			renderer2d->DrawCircle(Vec2(50, 50));
			renderer2d->SetCursor(Vec2(400, 200));
			renderer2d->SetBorderThickness(5.0f);
			renderer2d->DrawRoundedRect(Vec2(200, 100), Vec4(5, 15, 25, 35));

			renderer2d->PopFont();
		}
		renderer2d->End();

		const auto& renderer2dPackets = renderer2d->FlushDrawPackets(imageIndex);

		auto timeTaken = ((f32)(clock() - prevTime)) / CLOCKS_PER_SEC;

		for (auto drawPacket : renderer2dPackets)
		{
			drawList.AddDrawPacket(drawPacket);
		}

		// Finalize
		drawList.Finalize();
		scheduler->SetScopeDrawList("UI", &drawList.GetDrawListForTag(uiTag));
		//scheduler->SetScopeDrawList("Transparent", &drawList.GetDrawListForTag(transparentTag));
	}

	void WidgetSandbox::OnWindowResized(PlatformWindow* window, u32 newWidth, u32 newHeight)
	{
		rebuild = recompile = true;
	}

	void WidgetSandbox::OnWindowDestroyed(PlatformWindow* window)
	{
		if (window == this->mainWindow)
		{
			destroyed = true;
			this->mainWindow = nullptr;
		}
	}

} // namespace CE
