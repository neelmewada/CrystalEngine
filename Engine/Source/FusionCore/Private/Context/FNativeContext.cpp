#include "FusionCore.h"

namespace CE
{
	static Array<int> availableWindowIds{};
	static int currentWindowId = 0;

	FNativeContext::FNativeContext()
	{
		isIsolatedContext = true;
	}

	FNativeContext::~FNativeContext()
	{
		if (platformWindow)
		{
			
		}
	}

	FNativeContext* FNativeContext::Create(PlatformWindow* platformWindow, const String& name, FFusionContext* parentContext)
	{
		ZoneScoped;

		Object* outer = parentContext;
		if (outer == nullptr)
		{
			outer = FusionApplication::TryGet();
		}
		if (outer == nullptr)
		{
			return nullptr;
		}

		FNativeContext* nativeContext = CreateObject<FNativeContext>(outer, name);
		nativeContext->platformWindow = platformWindow;
		if (parentContext)
		{
			parentContext->AddChildContext(nativeContext);
		}
		nativeContext->Init();
		return nativeContext;
	}

	void FNativeContext::Init()
	{
		ZoneScoped;

		if (availableWindowIds.NonEmpty())
		{
			windowId = availableWindowIds[0];
			availableWindowIds.RemoveAt(0);
		}
		else
		{
			windowId = currentWindowId++;
		}

		attachmentId = String::Format("Window_{}", windowId);

		drawListTag = RPISystem::Get().GetDrawListTagRegistry()->AcquireTag(attachmentId);

		platformWindow->SetHitTestDelegate(MemberDelegate(&Self::WindowDragHitTest, this));

		RHI::SwapChainDescriptor desc{};
		desc.imageCount = FrameScheduler::Get()->GetFramesInFlight();
		desc.preferredFormats = { RHI::Format::R8G8B8A8_UNORM, RHI::Format::B8G8R8A8_UNORM };
		platformWindow->GetDrawableWindowSize(&desc.preferredWidth, &desc.preferredHeight);

		swapChain = RHI::gDynamicRHI->CreateSwapChain(platformWindow, desc);

		PlatformApplication::Get()->AddMessageHandler(this);

		renderer = CreateObject<FusionRenderer>(this, "FusionRenderer");

		UpdateViewConstants();

		FusionRendererInitInfo rendererInfo;
		rendererInfo.fusionShader = FusionApplication::Get()->GetFusionShader();
		rendererInfo.multisampling.sampleCount = 1;
		
		renderer->SetScreenSize(Vec2i(desc.preferredWidth, desc.preferredHeight));
		renderer->SetDrawListTag(drawListTag);

		renderer->Init(rendererInfo);

		painter = CreateObject<FPainter>(this, "FusionPainter");
		painter->renderer = renderer;

		FusionApplication::Get()->nativeWindows.Add(this);
	}

	void FNativeContext::OnQueuedDestroy()
	{
		if (IsDefaultInstance())
			return;

		Super::OnQueuedDestroy();

		FusionApplication::Get()->nativeWindows.Remove(this);

		if (drawListTag.IsValid())
		{
			RPISystem::Get().GetDrawListTagRegistry()->ReleaseTag(drawListTag);
		}

		drawListTag = 0;

		PlatformApplication::Get()->RemoveMessageHandler(this);

		if (platformWindow)
		{
			platformWindow->Hide();
		}
	}

	void FNativeContext::OnBeforeDestroy()
	{
		Super::OnBeforeDestroy();

		availableWindowIds.Add(windowId);
		windowId = -1;

		if (swapChain)
		{
			delete swapChain; swapChain = nullptr;
		}

		if (platformWindow)
		{
			PlatformApplication::Get()->DestroyWindow(platformWindow);
			platformWindow = nullptr;
		}
	}

	void FNativeContext::OnWindowResized(PlatformWindow* window, u32 newWidth, u32 newHeight)
	{
		ZoneScoped;

		if (!platformWindow || !renderer)
			return;
		if (platformWindow != window)
			return;

		availableSize = platformWindow->GetDrawableWindowSize().ToVec2();

		if (swapChain)
		{
			swapChain->Rebuild();
		}

		UpdateViewConstants();
		MarkLayoutDirty();

		NotifyWindowEvent(FEventType::NativeWindowResized, this);
	}

	void FNativeContext::OnWindowExposed(PlatformWindow* window)
	{
		ZoneScoped;

		if (!platformWindow || !renderer)
			return;
		if (platformWindow != window)
			return;

		const Vec2 newSize = platformWindow->GetDrawableWindowSize().ToVec2();

		//if (newSize != availableSize || childContexts.IsEmpty())
		if (false)
		{
			availableSize = newSize;

			if (swapChain)
			{
				swapChain->Rebuild();
			}
		}
		
		UpdateViewConstants();
		MarkLayoutDirty();

		NotifyWindowEvent(FEventType::NativeWindowExposed, this);
	}

	void FNativeContext::OnWindowMoved(PlatformWindow* window, int x, int y)
	{
		ZoneScoped;

		if (!platformWindow || !renderer)
			return;
		if (platformWindow != window)
			return;

		NotifyWindowEvent(FEventType::NativeWindowMoved, this);
	}

	void FNativeContext::OnWindowDestroyed(PlatformWindow* window)
	{
		if (platformWindow == window)
		{
			platformWindow = nullptr;
		}
	}

	void FNativeContext::UpdateViewConstants()
	{
		ZoneScoped;

		u32 screenWidth = 0; u32 screenHeight = 0;
		platformWindow->GetDrawableWindowSize(&screenWidth, &screenHeight);
		
		viewConstants.viewMatrix = Matrix4x4::Identity();

		viewConstants.projectionMatrix =
			Matrix4x4::Scale(Vec3(1.0f / screenWidth * 2, 1.0f / screenHeight * 2, 1)) *
			Matrix4x4::Translation(Vec3(-(f32)screenWidth * 0.5f, -(f32)screenHeight * 0.5f, 0));

		viewConstants.viewProjectionMatrix = viewConstants.projectionMatrix * viewConstants.viewMatrix;
		viewConstants.viewPosition = Vec4(0, 0, 0, 0);
		viewConstants.pixelResolution = Vec2(screenWidth, screenHeight);

		if (renderer)
		{
			renderer->SetViewConstants(viewConstants);
		}
	}

	bool FNativeContext::IsFocused() const
	{
		return platformWindow->IsFocused();
	}

	bool FNativeContext::IsShown() const
	{
		return platformWindow->IsShown();
	}

	void FNativeContext::TickInput()
	{
		ZoneScoped;

		Super::TickInput();

		
	}

	void FNativeContext::DoLayout()
	{
		ZoneScoped;

		availableSize = platformWindow->GetDrawableWindowSize().ToVec2();

		Super::DoLayout();
	}

	void FNativeContext::DoPaint()
	{
		ZoneScoped;

		Super::DoPaint();

		if (renderer && dirty)
		{
			renderer->Begin();

			if (painter && owningWidget)
			{
				owningWidget->OnPaint(painter);
			}

			for (int i = 0; i < localPopupStack.GetSize(); ++i)
			{
				localPopupStack[i]->OnPaint(painter);
			}

			renderer->End();

			dirty = false;
		}
	}

	void FNativeContext::PushNativePopup(FPopup* popup, Vec2 globalPosition, Vec2 size)
	{
		ZoneScoped;

		if (!popup)
			return;

		for (FFusionContext* context : childContexts)
		{
			if (context->owningWidget == popup)
				return;
		}

		PlatformWindowInfo windowInfo;
		windowInfo.fullscreen = windowInfo.hidden = windowInfo.maximised = windowInfo.resizable = false;
		windowInfo.windowFlags = PlatformWindowFlags::PopupMenu | PlatformWindowFlags::DestroyOnClose | PlatformWindowFlags::SkipTaskbar;

		PlatformWindow* window = PlatformApplication::Get()->CreatePlatformWindow(popup->GetName().GetString(), size.x, size.y, windowInfo);
		Vec2i windowPos = this->platformWindow->GetWindowPosition() + globalPosition.ToVec2i();
		window->SetWindowPosition(windowPos);
		window->SetBorderless(true);
		if (!popup->AutoClose() || popup->BlockInteraction())
		{
			window->SetAlwaysOnTop(true);
		}
		window->SetHitTestDelegate(MemberDelegate(&Self::WindowDragHitTest, this));

		FNativeContext* popupContext = Create(window, popup->GetName().GetString(), this);
		// Always set the Context in widgets before calculating layout
		popupContext->SetOwningWidget(popup);

		if (size.x <= 0 || size.y <= 0)
		{
			popup->CalculateIntrinsicSize();

			size = popup->GetIntrinsicSize();
		}

		popup->computedPosition = Vec2();
		popup->computedSize = size;
		popup->PlaceSubWidgets();

		window->SetWindowSize(Vec2i((int)size.x, (int)size.y));

		popup->isShown = true;
		popup->isNativePopup = true;

		AddChildContext(popupContext);

		GetRootContext()->SetFocusWidget(popup);
		popupContext->MarkLayoutDirty();
	}

	void FNativeContext::EmplaceFrameAttachments()
	{
		ZoneScoped;

		Super::EmplaceFrameAttachments();

		FrameScheduler* scheduler = FrameScheduler::Get();

		FrameAttachmentDatabase& attachmentDatabase = scheduler->GetAttachmentDatabase();

		attachmentDatabase.EmplaceFrameAttachment(attachmentId, swapChain);
	}

	void FNativeContext::EnqueueScopes()
	{
		ZoneScoped;

		Super::EnqueueScopes();

		FrameScheduler* scheduler = FrameScheduler::Get();

		FrameAttachmentDatabase& attachmentDatabase = scheduler->GetAttachmentDatabase();

		if (platformWindow != nullptr && !platformWindow->IsMinimized() && platformWindow->IsShown())
		{
			scheduler->BeginScope(attachmentId);
			{
				RHI::ImageScopeAttachmentDescriptor swapChainAttachment{};
				swapChainAttachment.attachmentId = attachmentId;
				if (clearScreen)
				{
					swapChainAttachment.loadStoreAction.clearValue = clearColor.ToVec4();
					swapChainAttachment.loadStoreAction.loadAction = RHI::AttachmentLoadAction::Clear;
				}
				else
				{
					swapChainAttachment.loadStoreAction.loadAction = RHI::AttachmentLoadAction::Load;
				}
				
				swapChainAttachment.loadStoreAction.storeAction = RHI::AttachmentStoreAction::Store;
				swapChainAttachment.multisampleState.sampleCount = 1;
				scheduler->UseAttachment(swapChainAttachment, RHI::ScopeAttachmentUsage::Color, RHI::ScopeAttachmentAccess::ReadWrite);

				scheduler->PresentSwapChain(swapChain);
			}
			scheduler->EndScope();
		}
	}

	void FNativeContext::SetDrawListMask(RHI::DrawListMask& outMask)
	{
		Super::SetDrawListMask(outMask);

		if (drawListTag)
		{
			outMask.Set(drawListTag);
		}
	}

	void FNativeContext::EnqueueDrawPackets(RHI::DrawListContext& drawList, u32 imageIndex)
	{
		ZoneScoped;

		Super::EnqueueDrawPackets(drawList, imageIndex);

		const auto& drawPackets = renderer->FlushDrawPackets(imageIndex);

		for (DrawPacket* drawPacket : drawPackets)
		{
			drawList.AddDrawPacket(drawPacket);
		}
	}

	void FNativeContext::SetDrawPackets(RHI::DrawListContext& drawList)
	{
		ZoneScoped;

		Super::SetDrawPackets(drawList);

		auto scheduler = FrameScheduler::Get();

		scheduler->SetScopeDrawList(attachmentId, &drawList.GetDrawListForTag(drawListTag));
	}

	void FNativeContext::OnWidgetDestroyed(FWidget* widget)
	{
		Super::OnWidgetDestroyed(widget);

		for (int i = 0; i < widgetsPressedPerMouseButton.GetSize(); i++)
		{
			if (widgetsPressedPerMouseButton[i] == widget)
			{
				widgetsPressedPerMouseButton[i] = nullptr;
			}
		}
	}

	FPainter* FNativeContext::GetPainter()
	{
		return painter;
	}

	Vec2 FNativeContext::GlobalToScreenSpacePosition(Vec2 pos)
	{
		return platformWindow->GetWindowPosition().ToVec2() + pos;
	}

	Vec2 FNativeContext::ScreenToGlobalSpacePosition(Vec2 pos)
	{
		return pos - platformWindow->GetWindowPosition().ToVec2();
	}

	void FNativeContext::Maximize()
	{
		if (isDestroyed)
			return;

		platformWindow->Maximize();
	}

	void FNativeContext::Minimize()
	{
		if (isDestroyed)
			return;

		platformWindow->Minimize();
	}

	void FNativeContext::Restore()
	{
		if (isDestroyed)
			return;

		platformWindow->Restore();
	}

	bool FNativeContext::IsMaximized()
	{
		if (isDestroyed)
			return false;

		return platformWindow->IsMaximized();
	}

	bool FNativeContext::IsMinimized()
	{
		if (isDestroyed)
			return false;

		return platformWindow->IsMinimized();
	}

	bool FNativeContext::WindowDragHitTest(PlatformWindow* window, Vec2 position)
	{
		if (!window->IsBorderless() || IsPopupWindow())
			return false;

		FWidget* hitWidget = HitTest(position);

		while (hitWidget != nullptr && !hitWidget->SupportsMouseEvents())
		{
			if (hitWidget->IsOfType<FTitleBar>())
			{
				return true;
			}

			hitWidget = hitWidget->parent;
		}

		return hitWidget != nullptr && hitWidget->IsOfType<FTitleBar>();
	}

} // namespace CE
