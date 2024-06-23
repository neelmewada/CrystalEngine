#include "FusionCore.h"

namespace CE
{

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

		attachmentId = String::Format("Window_{}", platformWindow->GetWindowId());

		drawListTag = RPISystem::Get().GetDrawListTagRegistry()->AcquireTag(attachmentId);

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

	void FNativeContext::OnBeforeDestroy()
	{
		Super::OnBeforeDestroy();

		if (IsDefaultInstance())
			return;

		FusionApplication::Get()->nativeWindows.Remove(this);

		if (drawListTag.IsValid())
		{
			RPISystem::Get().GetDrawListTagRegistry()->ReleaseTag(drawListTag);
		}

		drawListTag = 0;

		PlatformApplication::Get()->RemoveMessageHandler(this);

		FusionApplication::Get()->QueueDestroy(renderer);
		renderer = nullptr;

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

		if (swapChain)
		{
			swapChain->Rebuild();
		}

		UpdateViewConstants();

		MarkLayoutDirty();
	}

	void FNativeContext::OnWindowExposed(PlatformWindow* window)
	{
		ZoneScoped;

		if (!platformWindow || !renderer)
			return;
		if (platformWindow != window)
			return;

		if (swapChain)
		{
			swapChain->Rebuild();
		}

		UpdateViewConstants();

		MarkLayoutDirty();
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

	void FNativeContext::Tick()
	{
		ZoneScoped;

		Super::Tick();

		if (!owningWidget || FusionApplication::Get()->isExposed || !platformWindow->IsShown())
		{
			return;
		}

		// - Event Handling -

		Vec2 mousePos = (InputManager::GetGlobalMousePosition() - platformWindow->GetWindowPosition()).ToVec2();
		Vec2 mouseDelta = InputManager::GetMouseDelta().ToVec2();
		Vec2 wheelDelta = InputManager::GetMouseWheelDelta();
		if (prevMousePos == Vec2())
			prevMousePos = mousePos;

		Rect windowRect = Rect::FromSize(Vec2(), platformWindow->GetDrawableWindowSize().ToVec2());

		MouseButtonMask curButtonMask = MouseButtonMask::None;
		if (InputManager::IsMouseButtonHeld(MouseButton::Left))
		{
			curButtonMask |= MouseButtonMask::Left;
		}
		if (InputManager::IsMouseButtonHeld(MouseButton::Right))
		{
			curButtonMask |= MouseButtonMask::Right;
		}
		if (InputManager::IsMouseButtonHeld(MouseButton::Middle))
		{
			curButtonMask |= MouseButtonMask::Middle;
		}
		if (InputManager::IsMouseButtonHeld(MouseButton::Button4))
		{
			curButtonMask |= MouseButtonMask::Button4;
		}
		if (InputManager::IsMouseButtonHeld(MouseButton::Button5))
		{
			curButtonMask |= MouseButtonMask::Button5;
		}

		Enum* keyCodeEnum = GetStaticEnum<KeyCode>();
		Enum* keyModifierEnum = GetStaticEnum<KeyModifier>();

		keyModifierStates = KeyModifier::None;
		keyPressStates.Reset();

		for (int i = 0; i < keyModifierEnum->GetConstantsCount(); ++i)
		{
			if (InputManager::TestModifiers((KeyModifier)keyModifierEnum->GetConstant(i)->GetValue()))
			{
				keyModifierStates |= (KeyModifier)keyModifierEnum->GetConstant(i)->GetValue();
			}
		}

		FWidget* hoveredWidget = nullptr;

		if (windowRect.Contains(mousePos))
		{
			hoveredWidget = owningWidget->HitTest(mousePos);
		}

		if (!platformWindow->IsFocused())
		{
			hoveredWidget = nullptr;
		}

		if (!hoveredWidgetStack.IsEmpty() && hoveredWidgetStack.Top() != hoveredWidget &&
			(hoveredWidget == nullptr || !hoveredWidgetStack.Top()->ChildExistsRecursive(hoveredWidget)))
		{
			FMouseEvent event{};
			event.type = FEventType::MouseLeave;
			event.mousePosition = mousePos;
			event.prevMousePosition = prevMousePos;
			event.buttons = curButtonMask;
			event.direction = FEventDirection::BottomToTop;
			event.isInside = true;
			event.keyModifiers = keyModifierStates;

			while (hoveredWidgetStack.NonEmpty() && !hoveredWidgetStack.Top()->ChildExistsRecursive(hoveredWidget))
			{
				event.sender = hoveredWidgetStack.Top();
				event.Reset();

				if (event.sender->SupportsMouseEvents())
				{
					event.sender->HandleEvent(&event);
				}
				hoveredWidgetStack.Pop();
			}
		}

		if (hoveredWidget != nullptr && 
			(hoveredWidgetStack.IsEmpty() || hoveredWidgetStack.Top() != hoveredWidget) &&
			(hoveredWidgetStack.IsEmpty() || !hoveredWidget->ChildExistsRecursive(hoveredWidgetStack.Top())))
		{
			FMouseEvent event{};
			event.type = FEventType::MouseEnter;
			event.mousePosition = mousePos;
			event.prevMousePosition = prevMousePos;
			event.buttons = curButtonMask;
			event.direction = FEventDirection::BottomToTop;
			event.isInside = true;
			event.keyModifiers = keyModifierStates;

			int idx = hoveredWidgetStack.GetSize();
			FWidget* basePrevWidget = nullptr;
			if (hoveredWidgetStack.NonEmpty())
				basePrevWidget = hoveredWidgetStack.Top();

			auto widget = hoveredWidget;

			while ((hoveredWidgetStack.IsEmpty() || widget != basePrevWidget) && widget != nullptr)
			{
				hoveredWidgetStack.InsertAt(idx, widget);
				widget = widget->parent;
			}

			for (int i = idx; i < hoveredWidgetStack.GetSize(); ++i)
			{
				event.sender = hoveredWidgetStack[i];
				event.Reset();

				event.sender->HandleEvent(&event);
			}
		}

		// - Mouse Click Events -

		if (abs(mouseDelta.x) >= FLT_EPSILON || abs(mouseDelta.y) >= FLT_EPSILON)
		{
			FMouseEvent mouseEvent{};
			mouseEvent.type = FEventType::MouseMove;
			mouseEvent.buttons = curButtonMask;
			mouseEvent.mousePosition = mousePos;
			mouseEvent.prevMousePosition = prevMousePos;
			mouseEvent.direction = FEventDirection::BottomToTop;
			mouseEvent.wheelDelta = wheelDelta;
			mouseEvent.keyModifiers = keyModifierStates;

			if (hoveredWidgetStack.NonEmpty())
			{
				mouseEvent.sender = hoveredWidgetStack.Top();
				hoveredWidgetStack.Top()->HandleEvent(&mouseEvent);
			}

			if (draggedWidget != nullptr && curButtonMask != MouseButtonMask::None)
			{
				FDragEvent dragEvent{};
				dragEvent.type = FEventType::DragMove;
				dragEvent.buttons = curButtonMask;
				dragEvent.mousePosition = mousePos;
				dragEvent.prevMousePosition = prevMousePos;
				dragEvent.direction = FEventDirection::BottomToTop;
				dragEvent.wheelDelta = wheelDelta;
				dragEvent.isInside = true;
				dragEvent.keyModifiers = keyModifierStates;

				dragEvent.sender = nullptr;
				if (hoveredWidgetStack.NonEmpty())
					dragEvent.sender = hoveredWidgetStack.Top();
				dragEvent.draggedWidget = draggedWidget;

				draggedWidget->HandleEvent(&dragEvent);

				// Cancel dragging if any of the FDragEvent's are not consumed by the dragged widget
				if (!dragEvent.isConsumed)
				{
					draggedWidget = nullptr;
				}
				else
				{
					draggedWidget = dragEvent.draggedWidget;
				}
			}
		}

		Enum* mouseButtonEnum = GetStaticEnum<MouseButton>();
		for (int i = 0; i < mouseButtonEnum->GetConstantsCount(); ++i)
		{
			MouseButton mouseButton = (MouseButton)mouseButtonEnum->GetConstant(i)->GetValue();
			if (InputManager::IsMouseButtonDown(mouseButton))
			{
				FMouseEvent event{};
				event.type = FEventType::MousePress;
				event.mousePosition = mousePos;
				event.prevMousePosition = prevMousePos;
				event.buttons = (MouseButtonMask)BIT((int)mouseButton);
				event.direction = FEventDirection::BottomToTop;
				event.keyModifiers = keyModifierStates;
				event.isDoubleClick = InputManager::GetMouseButtonClicks(mouseButton) == 2;

				if (hoveredWidgetStack.NonEmpty())
				{
					event.sender = hoveredWidgetStack.Top();
					widgetToFocus = event.sender;

					while (event.sender != nullptr && !event.sender->SupportsMouseEvents())
					{
						event.sender = event.sender->parent;
					}

					if (event.sender != nullptr)
					{
						widgetsPressedPerMouseButton[i] = event.sender;

						event.sender->HandleEvent(&event);
					}

					FWidget* dragEventWidget = hoveredWidgetStack.Top();

					while (dragEventWidget != nullptr)
					{
						if (dragEventWidget->SupportsFocusEvents())
						{
							FDragEvent dragEvent{};
							dragEvent.type = FEventType::DragBegin;
							dragEvent.mousePosition = mousePos;
							dragEvent.prevMousePosition = prevMousePos;
							dragEvent.buttons = (MouseButtonMask)BIT((int)mouseButton);
							dragEvent.direction = FEventDirection::BottomToTop;
							dragEvent.keyModifiers = keyModifierStates;
							dragEvent.isDoubleClick = InputManager::GetMouseButtonClicks(mouseButton) == 2;

							dragEvent.sender = dragEventWidget;
							dragEvent.draggedWidget = dragEventWidget;
							dragEvent.sender->HandleEvent(&dragEvent);

							if (dragEvent.isConsumed)
							{
								draggedWidget = dragEvent.draggedWidget;
							}

							break;
						}

						dragEventWidget = dragEventWidget->parent;
					}
				}
			}

			if (InputManager::IsMouseButtonUp(mouseButton))
			{
				FMouseEvent event{};
				event.type = FEventType::MouseRelease;
				event.mousePosition = mousePos;
				event.prevMousePosition = prevMousePos;
				event.buttons = (MouseButtonMask)BIT((int)mouseButton);
				event.direction = FEventDirection::BottomToTop;
				event.isInside = true;
				event.keyModifiers = keyModifierStates;

				if (hoveredWidgetStack.NonEmpty())
				{
					event.sender = hoveredWidgetStack.Top();
					while (event.sender != nullptr && !event.sender->SupportsMouseEvents())
					{
						event.sender = event.sender->parent;
					}

					if (event.sender != nullptr)
					{
						event.sender->HandleEvent(&event);
					}
				}

				if (widgetsPressedPerMouseButton[i] != nullptr && widgetsPressedPerMouseButton[i] != event.sender)
				{
					event.Reset();
					event.isInside = false;
					widgetsPressedPerMouseButton[i]->HandleEvent(&event);
				}

				widgetsPressedPerMouseButton[i] = nullptr;
			}
		}
		
		if (hoveredWidget != prevHoveredWidget)
		{
			/*if (hoveredWidget != nullptr)
				CE_LOG(Info, All, "Hover Changed: {}", hoveredWidget->GetName());
			else
				CE_LOG(Info, All, "Hover Changed: NULL");*/

			prevHoveredWidget = hoveredWidget;
		}

		// - Focus Events -

		if (owningWidget && IsFocused() && !owningWidget->IsFocused())
		{
			FFocusEvent focusEvent{};
			focusEvent.gotFocus = true;
			focusEvent.type = FEventType::FocusChanged;
			focusEvent.sender = owningWidget;
			focusEvent.focusedWidget = owningWidget;
			focusEvent.direction = FEventDirection::BottomToTop;

			owningWidget->HandleEvent(&focusEvent);
		}
		else if (owningWidget && !IsFocused() && owningWidget->IsFocused())
		{
			FFocusEvent focusEvent{};
			focusEvent.gotFocus = false;
			focusEvent.type = FEventType::FocusChanged;
			focusEvent.sender = owningWidget;
			focusEvent.focusedWidget = nullptr;
			focusEvent.direction = FEventDirection::TopToBottom;

			owningWidget->HandleEvent(&focusEvent);

			curFocusWidget = widgetToFocus = nullptr;
		}

		if (curFocusWidget != nullptr && !curFocusWidget->IsFocused() && curFocusWidget == widgetToFocus)
		{
			curFocusWidget = nullptr;
			widgetToFocus = nullptr;
		}

		if (widgetToFocus != curFocusWidget)
		{
			if (curFocusWidget != nullptr && (widgetToFocus == nullptr || !widgetToFocus->ParentExistsRecursive(curFocusWidget)))
			{
				FWidget* base = curFocusWidget;

				while (base != nullptr)
				{
					if (base->parent != nullptr && widgetToFocus->ParentExistsRecursive(base->parent))
					{
						break;
					}
					base = base->parent;
				}

				if (base != nullptr)
				{
					FFocusEvent focusEvent{};
					focusEvent.type = FEventType::FocusChanged;
					focusEvent.sender = base;
					focusEvent.focusedWidget = widgetToFocus;
					focusEvent.gotFocus = false;
					focusEvent.direction = FEventDirection::TopToBottom;

					base->HandleEvent(&focusEvent);
				}
				else
				{
					FFocusEvent focusEvent{};
					focusEvent.type = FEventType::FocusChanged;
					focusEvent.sender = curFocusWidget;
					focusEvent.focusedWidget = nullptr;
					focusEvent.gotFocus = false;
					focusEvent.direction = FEventDirection::BottomToTop;

					curFocusWidget->HandleEvent(&focusEvent);
				}
			}
			
			if (widgetToFocus != nullptr)
			{
				FFocusEvent focusEvent{};
				focusEvent.type = FEventType::FocusChanged;
				focusEvent.sender = focusEvent.focusedWidget = widgetToFocus;
				focusEvent.gotFocus = true;
				focusEvent.direction = FEventDirection::BottomToTop;

				widgetToFocus->HandleEvent(&focusEvent);
			}

			curFocusWidget = widgetToFocus;
		}

		FWidget* keyEventWidget = curFocusWidget;
		while (keyEventWidget != nullptr && !keyEventWidget->SupportsKeyboardEvents())
		{
			keyEventWidget = keyEventWidget->parent;
		}

		if (keyEventWidget != nullptr)
		{
			for (int i = 0; i < keyCodeEnum->GetConstantsCount(); ++i)
			{
				KeyCode keyCode = (KeyCode)keyCodeEnum->GetConstant(i)->GetValue();

				bool isDown = InputManager::IsKeyDown(keyCode);
				bool isUp = InputManager::IsKeyUp(keyCode);

				bool isHeldDelayed = InputManager::IsKeyHeldDelayed(keyCode);

				FKeyEvent keyEvent{};
				keyEvent.modifiers = keyModifierStates;
				keyEvent.key = keyCode;
				keyEvent.sender = keyEventWidget;

				if (isDown && !keyPressStates.Test(i))
				{
					keyEvent.type = FEventType::KeyPress;

					keyPressStates.Set(i, true);

					keyEventWidget->HandleEvent(&keyEvent);
				}
				else if (isUp && keyPressStates.Test(i))
				{
					keyEvent.type = FEventType::KeyRelease;

					keyPressStates.Set(i, false);

					keyEventWidget->HandleEvent(&keyEvent);
				}

				if (isHeldDelayed)
				{
					keyEvent.type = FEventType::KeyHeld;

					keyEventWidget->HandleEvent(&keyEvent);
				}
			}
		}
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

			renderer->End();

			dirty = false;
		}
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

} // namespace CE
