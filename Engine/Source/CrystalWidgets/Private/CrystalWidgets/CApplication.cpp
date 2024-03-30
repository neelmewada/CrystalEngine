#include "CrystalWidgets.h"

namespace CE::Widgets
{
	static CApplication* instance = nullptr;

	CApplication::CApplication()
	{
		if (IsDefaultInstance())
			return;
		
		CE_ASSERT(instance == nullptr, "Only 1 instance of CApplication is allowed");
		instance = this;
	}

	CApplication::~CApplication()
	{
		if (instance == this)
		{
			instance = nullptr;
		}
	}

	CApplication* CApplication::Get()
	{
		if (instance == nullptr)
		{
			instance = CreateObject<CApplication>(nullptr, "Application", OF_Transient);
		}
		return instance;
	}

	CApplication* CApplication::TryGet()
	{
		return instance;
	}

	void CApplication::Initialize(const CApplicationInitInfo& initInfo)
	{
		draw2dShader = initInfo.draw2dShader;
		defaultFontName = initInfo.defaultFontName;
		defaultFont = initInfo.defaultFont;
		numFramesInFlight = initInfo.numFramesInFlight;
		scheduler = initInfo.scheduler;

		registeredFonts[defaultFontName] = initInfo.defaultFont;

		PlatformApplication::Get()->AddMessageHandler(this);
	}

	void CApplication::Shutdown()
	{
		PlatformApplication::Get()->RemoveMessageHandler(this);
	}

	void CApplication::Tick()
	{
		constexpr u32 destroyAfterFrames = 8;

		for (int i = destructionQueue.GetSize() - 1; i >= 0; --i)
		{
			destructionQueue[i]->destroyFrameCounter++;
			if (destructionQueue[i]->destroyFrameCounter > destroyAfterFrames)
			{
				destructionQueue[i]->Destroy();
				destructionQueue.RemoveAt(i);
			}
		}

		for (CTimer* timer : timers)
		{
			timer->Tick();
		}

		// Global events

		Vec2 globalMousePos = InputManager::GetGlobalMousePosition().ToVec2();
		Vec2 mouseDelta = InputManager::GetMouseDelta().ToVec2();
		Vec2 mouseWheelDelta = InputManager::GetMouseWheelDelta();
		if (prevMousePos == Vec2())
			prevMousePos = globalMousePos;

		MouseButton curButton = MouseButton::None;
		if (InputManager::IsMouseButtonHeld(MouseButton::Left))
		{
			curButton = MouseButton::Left;
		}
		else if (InputManager::IsMouseButtonHeld(MouseButton::Right))
		{
			curButton = MouseButton::Right;
		}
		else if (InputManager::IsMouseButtonHeld(MouseButton::Middle))
		{
			curButton = MouseButton::Middle;
		}

		std::function<CWidget* (CWidget*)> getBottomMostHoveredWidget = [&](CWidget* widget) -> CWidget*
			{
				if (widget == nullptr || !widget->IsEnabled() || !widget->interactable)
					return nullptr;

				Rect widgetRect = widget->GetScreenSpaceRect();

				CWindow* ownerWindow = widget->ownerWindow;
				if (ownerWindow)
				{
					if (PlatformWindow* nativeWindow = ownerWindow->GetRootNativeWindow())
					{
						if (!nativeWindow->IsFocussed() || !nativeWindow->IsShown() || nativeWindow->IsMinimized())
						{
							return nullptr;
						}
					}
				}

				if (widgetRect.Contains(globalMousePos))
				{
					for (int i = widget->attachedWidgets.GetSize() - 1; i >= 0; --i)
					{
						if (!widget->attachedWidgets[i]->IsEnabled())
							continue;
						CWidget* insideWidget = getBottomMostHoveredWidget(widget->attachedWidgets[i]);
						if (insideWidget && insideWidget->receiveMouseEvents)
							return insideWidget;
					}

					return widget;
				}

				return nullptr;
			};

		CWidget* hoveredWidget = nullptr;
		CWindow* hoveredWindow = nullptr;

		for (int i = 0; i < windows.GetSize(); ++i)
		{
			hoveredWidget = getBottomMostHoveredWidget(windows[i]);
			if (hoveredWidget)
			{
				hoveredWindow = hoveredWidget->ownerWindow;
				break;
			}
		}

		if (hoveredWidget)
		{
			PlatformWindow* platformWindow = hoveredWidget->GetNativeWindow();
			if (platformWindow && !platformWindow->IsFocussed())
			{
				hoveredWidget = nullptr;
			}
		}

		if (hoveredWidgetsStack.NonEmpty() && hoveredWidgetsStack.Top() != hoveredWidget &&
			(hoveredWidget == nullptr || !hoveredWidgetsStack.Top()->SubWidgetExistsRecursive(hoveredWidget)))
		{
			CMouseEvent mouseEvent{};
			mouseEvent.name = "MouseLeave";
			mouseEvent.type = CEventType::MouseLeave;
			mouseEvent.button = curButton;
			mouseEvent.mousePos = globalMousePos;
			mouseEvent.prevMousePos = prevMousePos;
			mouseEvent.direction = CEventDirection::BottomToTop;
			mouseEvent.wheelDelta = mouseWheelDelta;

			while (hoveredWidgetsStack.NonEmpty() && hoveredWidgetsStack.Top() != hoveredWidget)
			{
				mouseEvent.sender = hoveredWidgetsStack.Top();
				mouseEvent.Reset();
				if (hoveredWidgetsStack.Top()->receiveMouseEvents)
				{
					hoveredWidgetsStack.Top()->HandleEvent(&mouseEvent);
				}
				hoveredWidgetsStack.Pop();
			}
		}

		if (hoveredWidget != nullptr && (hoveredWidgetsStack.IsEmpty() || hoveredWidgetsStack.Top() != hoveredWidget) &&
			(hoveredWidgetsStack.IsEmpty() || !hoveredWidget->SubWidgetExistsRecursive(hoveredWidgetsStack.Top())))
		{
			CMouseEvent mouseEvent{};
			mouseEvent.name = "MouseEnter";
			mouseEvent.type = CEventType::MouseEnter;
			mouseEvent.button = curButton;
			mouseEvent.mousePos = globalMousePos;
			mouseEvent.prevMousePos = prevMousePos;
			mouseEvent.direction = CEventDirection::BottomToTop;
			mouseEvent.wheelDelta = mouseWheelDelta;

			int idx = hoveredWidgetsStack.GetSize();
			CWidget* basePrevWidget = nullptr;
			if (hoveredWidgetsStack.NonEmpty())
				basePrevWidget = hoveredWidgetsStack.Top();

			auto widget = hoveredWidget;

			while ((hoveredWidgetsStack.IsEmpty() || widget != basePrevWidget) && widget != nullptr)
			{
				hoveredWidgetsStack.InsertAt(idx, widget);
				widget = widget->parent;
			}

			for (int i = idx; i < hoveredWidgetsStack.GetSize(); i++)
			{
				mouseEvent.Reset();
				mouseEvent.sender = hoveredWidgetsStack[i];
				if (hoveredWidgetsStack[i]->receiveMouseEvents)
				{
					hoveredWidgetsStack[i]->HandleEvent(&mouseEvent);
				}
			}
		}

		if (abs(mouseWheelDelta.x) >= FLT_EPSILON || abs(mouseWheelDelta.y) >= FLT_EPSILON)
		{
			CMouseEvent mouseEvent{};
			mouseEvent.name = "MouseWheel";
			mouseEvent.type = CEventType::MouseWheel;
			mouseEvent.button = curButton;
			mouseEvent.mousePos = globalMousePos;
			mouseEvent.prevMousePos = prevMousePos;
			mouseEvent.direction = CEventDirection::BottomToTop;
			mouseEvent.wheelDelta = mouseWheelDelta;

			if (hoveredWidgetsStack.NonEmpty())
			{
				mouseEvent.sender = hoveredWidgetsStack.Top();
				hoveredWidgetsStack.Top()->HandleEvent(&mouseEvent);
			}
		}

		if (abs(mouseDelta.x) >= FLT_EPSILON || abs(mouseDelta.y) >= FLT_EPSILON)
		{
			CMouseEvent mouseEvent{};
			mouseEvent.name = "MouseMove";
			mouseEvent.type = CEventType::MouseMove;
			mouseEvent.button = curButton;
			mouseEvent.mousePos = globalMousePos;
			mouseEvent.prevMousePos = prevMousePos;
			mouseEvent.direction = CEventDirection::BottomToTop;
			mouseEvent.wheelDelta = mouseWheelDelta;

			if (hoveredWidgetsStack.NonEmpty())
			{
				mouseEvent.sender = hoveredWidgetsStack.Top();
				hoveredWidgetsStack.Top()->HandleEvent(&mouseEvent);
			}

			if (draggedWidget != nullptr)
			{
				CDragEvent dragEvent{};
				dragEvent.name = "DragEvent";
				dragEvent.type = CEventType::DragMove;
				dragEvent.button = MouseButton::Left;
				dragEvent.mousePos = globalMousePos;
				dragEvent.prevMousePos = prevMousePos;
				dragEvent.direction = CEventDirection::BottomToTop;
				dragEvent.wheelDelta = mouseWheelDelta;
				dragEvent.isInside = true;

				dragEvent.sender = nullptr;
				if (hoveredWidgetsStack.NonEmpty())
					dragEvent.sender = hoveredWidgetsStack.Top();
				dragEvent.draggedWidget = draggedWidget;

				draggedWidget->HandleEvent(&dragEvent);

				// Cancel dragging if any of the CDragEvent's are not consumed by the dragged widget
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

		// - Mouse Click Events -

		Enum* mouseButtonEnum = GetStaticEnum<MouseButton>();
		for (int i = 0; i < mouseButtonEnum->GetConstantsCount(); ++i)
		{
			MouseButton mouseButton = (MouseButton)mouseButtonEnum->GetConstant(i)->GetValue();
			if (InputManager::IsMouseButtonDown(mouseButton))
			{
				CMouseEvent event{};
				event.name = "MousePress";
				event.type = CEventType::MousePress;
				event.button = mouseButton;
				event.mousePos = globalMousePos;
				event.prevMousePos = prevMousePos;
				event.direction = CEventDirection::BottomToTop;
				event.isInside = true;
				event.wheelDelta = mouseWheelDelta;
				event.isDoubleClick = InputManager::GetMouseButtonClicks(mouseButton) == 2;

				if (hoveredWidgetsStack.NonEmpty())
				{
					focusWidget = hoveredWidgetsStack.Top();
					
					event.sender = hoveredWidgetsStack.Top();
					widgetsPressedPerMouseButton[i] = event.sender;
					hoveredWidgetsStack.Top()->HandleEvent(&event);

					if (hoveredWidgetsStack.Top()->receiveDragEvents && mouseButton == MouseButton::Left)
					{
						CDragEvent dragEvent{};
						dragEvent.name = "DragEvent";
						dragEvent.type = CEventType::DragBegin;
						dragEvent.button = mouseButton;
						dragEvent.mousePos = globalMousePos;
						dragEvent.prevMousePos = prevMousePos;
						dragEvent.direction = CEventDirection::BottomToTop;
						dragEvent.isInside = true;
						dragEvent.wheelDelta = mouseWheelDelta;

						dragEvent.sender = hoveredWidgetsStack.Top();
						dragEvent.draggedWidget = hoveredWidgetsStack.Top();
						hoveredWidgetsStack.Top()->HandleEvent(&dragEvent);

						if (dragEvent.isConsumed)
						{
							draggedWidget = dragEvent.draggedWidget;
						}
					}
				}
			}

			if (InputManager::IsMouseButtonUp(mouseButton))
			{
				CMouseEvent event{};
				event.name = "MouseRelease";
				event.type = CEventType::MouseRelease;
				event.button = mouseButton;
				event.mousePos = globalMousePos;
				event.prevMousePos = prevMousePos;
				event.direction = CEventDirection::BottomToTop;
				event.isInside = true;
				event.wheelDelta = mouseWheelDelta;

				if (hoveredWidgetsStack.NonEmpty())
				{
					event.sender = hoveredWidgetsStack.Top();
					hoveredWidgetsStack.Top()->HandleEvent(&event);
				}

				if (widgetsPressedPerMouseButton[i] != nullptr && widgetsPressedPerMouseButton[i] != event.sender)
				{
					event.Reset();
					event.isInside = false;
					widgetsPressedPerMouseButton[i]->HandleEvent(&event);
				}

				if (draggedWidget != nullptr)
				{
					CDragEvent dragEvent{};
					dragEvent.name = "DragEvent";
					dragEvent.type = CEventType::DragEnd;
					dragEvent.button = mouseButton;
					dragEvent.mousePos = globalMousePos;
					dragEvent.prevMousePos = prevMousePos;
					dragEvent.direction = CEventDirection::BottomToTop;
					dragEvent.isInside = true;
					dragEvent.wheelDelta = mouseWheelDelta;

					dragEvent.sender = nullptr;
					if (hoveredWidgetsStack.NonEmpty())
						dragEvent.sender = hoveredWidgetsStack.Top();
					dragEvent.draggedWidget = draggedWidget;
					dragEvent.dropTarget = dragEvent.sender;
					draggedWidget->HandleEvent(&dragEvent);

					draggedWidget = nullptr;
				}

				widgetsPressedPerMouseButton[i] = nullptr;
			}
		}

		// - Focus Events -

		if (curFocusedWidget != nullptr && !curFocusedWidget->IsFocussed() && curFocusedWidget == focusWidget)
		{
			curFocusedWidget = nullptr;
			focusWidget = nullptr;
		}

		if (focusWidget != curFocusedWidget)
		{
			if (curFocusedWidget != nullptr)
			{
				CFocusEvent focusLostEvent{};
				focusLostEvent.name = "LostFocus";
				focusLostEvent.type = CEventType::FocusChanged;
				focusLostEvent.sender = curFocusedWidget;
				focusLostEvent.gotFocus = false;
				focusLostEvent.focusedWidget = focusWidget;

				curFocusedWidget->HandleEvent(&focusLostEvent);
			}

			if (focusWidget != nullptr)
			{
				CFocusEvent focusEvent{};
				focusEvent.name = "GotFocus";
				focusEvent.type = CEventType::FocusChanged;
				focusEvent.sender = focusEvent.focusedWidget = focusWidget;
				focusEvent.gotFocus = true;

				focusEvent.sender = focusWidget;

				focusWidget->HandleEvent(&focusEvent);
			}

			curFocusedWidget = focusWidget;
		}

		prevMousePos = globalMousePos;

		Enum* keyCodeEnum = GetStaticEnum<KeyCode>();
		Enum* keyModifierEnum = GetStaticEnum<KeyModifier>();

		keyModifierStates = KeyModifier::None;
		keyPressStates.Resize(keyCodeEnum->GetConstantsCount());

		CWidget* keyEventWidget = curFocusedWidget;
		while (keyEventWidget != nullptr && !keyEventWidget->receiveKeyEvents)
		{
			keyEventWidget = keyEventWidget->parent;
		}

		if (keyEventWidget)
		{
			for (int i = 0; i < keyModifierEnum->GetConstantsCount(); ++i)
			{
				if (InputManager::TestModifiers((KeyModifier)keyModifierEnum->GetConstant(i)->GetValue()))
				{
					keyModifierStates |= (KeyModifier)keyModifierEnum->GetConstant(i)->GetValue();
				}
			}

			for (int i = 0; i < keyCodeEnum->GetConstantsCount(); ++i)
			{
				KeyCode keyCode = (KeyCode)keyCodeEnum->GetConstant(i)->GetValue();

				bool isDown = InputManager::IsKeyDown(keyCode);
				bool isUp = InputManager::IsKeyUp(keyCode);

				bool isHeldDelayed = InputManager::IsKeyHeldDelayed(keyCode);

				CKeyEvent keyEvent{};
				keyEvent.name = "KeyEvent";
				keyEvent.modifier = keyModifierStates;
				keyEvent.key = keyCode;
				keyEvent.sender = keyEventWidget;

				if (isDown && !keyPressStates[i])
				{
					keyEvent.type = CEventType::KeyPress;

					keyPressStates[i] = true;

					keyEventWidget->HandleEvent(&keyEvent);
				}
				else if (isUp && keyPressStates[i])
				{
					keyEvent.type = CEventType::KeyRelease;

					keyPressStates[i] = false;

					keyEventWidget->HandleEvent(&keyEvent);
				}

				if (isHeldDelayed)
				{
					keyEvent.type = CEventType::KeyHeld;

					keyEventWidget->HandleEvent(&keyEvent);
				}
			}
		}

		// Per window events inside Tick()

		for (int i = 0; i < windows.GetSize(); i++)
		{
			windows[i]->Tick();
		}
	}

	CWindow* CApplication::CreateWindow(const String& name, const String& title, PlatformWindow* nativeWindow)
	{
		CWindow* window = CreateObject<CWindow>(this, name);
		window->SetPlatformWindow(nativeWindow);
		window->SetTitle(title);
		return window;
	}

	void CApplication::RegisterFont(Name fontName, RPI::FontAtlasAsset* fontAtlas)
	{
		registeredFonts[fontName] = fontAtlas;
		
		for (auto window : windows)
		{
			if (window->renderer != nullptr)
			{
				window->renderer->RegisterFont(fontName, fontAtlas);
			}
		}
	}

	void CApplication::SetGlobalStyleSheet(CStyleSheet* globalStyleSheet)
	{
		this->globalStyleSheet = globalStyleSheet;
	}

	void CApplication::LoadGlobalStyleSheet(const IO::Path& path)
	{
		globalStyleSheet = CSSStyleSheet::Load(path, this);
	}

	void CApplication::BuildFrameGraph()
	{
		for (int i = 0; i < windows.GetSize(); ++i)
		{
			PlatformWindow* platformWindow = windows[i]->nativeWindow;

			if (!platformWindow)
				continue;

			FrameAttachmentDatabase& attachmentDatabase = scheduler->GetAttachmentDatabase();

			Name id = String::Format("{}", platformWindow->GetWindowId());

			attachmentDatabase.EmplaceFrameAttachment(id, windows[i]->swapChain);

			if (!platformWindow->IsMinimized() && platformWindow->IsShown())
			{
				scheduler->BeginScope(id);
				{
					RHI::ImageScopeAttachmentDescriptor swapChainAttachment{};
					swapChainAttachment.attachmentId = id;
					swapChainAttachment.loadStoreAction.clearValue = Vec4(0, 0, 0, 1);
					swapChainAttachment.loadStoreAction.loadAction = RHI::AttachmentLoadAction::Clear;
					swapChainAttachment.loadStoreAction.storeAction = RHI::AttachmentStoreAction::Store;
					swapChainAttachment.multisampleState.sampleCount = 1;
					scheduler->UseAttachment(swapChainAttachment, RHI::ScopeAttachmentUsage::Color, RHI::ScopeAttachmentAccess::Write);

					scheduler->PresentSwapChain(windows[i]->swapChain);
				}
				scheduler->EndScope();
			}
		}
	}

	void CApplication::SetDrawListMasks(RHI::DrawListMask& outMask)
	{
		for (int i = 0; i < windows.GetSize(); ++i)
		{
			outMask.Set(windows[i]->GetDrawListTag());
		}
	}

	void CApplication::FlushDrawPackets(RHI::DrawListContext& drawList, u32 imageIndex)
	{
		for (int i = 0; i < windows.GetSize(); ++i)
		{
			if (!windows[i]->IsVisible() || !windows[i]->IsEnabled())
				continue;

			const auto& packets = windows[i]->FlushDrawPackets(imageIndex);

			for (RHI::DrawPacket* drawPacket : packets)
			{
				drawList.AddDrawPacket(drawPacket);
			}
		}
	}

	void CApplication::SubmitDrawPackets(RHI::DrawListContext& drawList)
	{
		for (int i = 0; i < windows.GetSize(); ++i)
		{
			PlatformWindow* platformWindow = windows[i]->nativeWindow;

			if (!platformWindow)
				continue;

			Name id = String::Format("{}", platformWindow->GetWindowId());

			scheduler->SetScopeDrawList(id, &drawList.GetDrawListForTag(windows[i]->GetDrawListTag()));
		}
	}

	void CApplication::OnWindowDestroyed(PlatformWindow* nativeWindow)
	{
		for (int i = windows.GetSize() - 1; i >= 0; --i)
		{
			if (windows[i]->nativeWindow == nativeWindow)
			{
				windows[i]->Destroy();
				windows.RemoveAt(i);
			}
		}
	}

	void CApplication::OnWindowCreated(PlatformWindow* window)
	{

	}

	void CApplication::OnWidgetDestroyed(CWidget* widget)
	{
		for (int i = 0; i < widgetsPressedPerMouseButton.GetSize(); i++)
		{
			if (widgetsPressedPerMouseButton[i] == widget)
			{
				widgetsPressedPerMouseButton[i] = nullptr;
			}
		}
	}

	void CApplication::OnWindowResized(PlatformWindow* nativeWindow, u32 newWidth, u32 newHeight)
	{
		for (CWindow* window : windows)
		{
			if (window->nativeWindow == nativeWindow)
			{
				window->SetNeedsStyle();
				window->SetNeedsLayout();
				window->SetNeedsPaint();
			}
		}
	}

	void CApplication::PushCursor(CCursor cursor)
	{
		cursorStack.Push(cursor);
		PlatformApplication::Get()->SetSystemCursor(ToSystemCursor(cursor));
	}

	CCursor CApplication::GetTopCursor()
	{
		if (cursorStack.NonEmpty())
			return cursorStack.Top();
		return CCursor::Arrow;
	}

	void CApplication::PopCursor()
	{
		if (cursorStack.IsEmpty())
		{
			PlatformApplication::Get()->SetSystemCursor(SystemCursor::Arrow);
			return;
		}

		cursorStack.Pop();
		if (cursorStack.IsEmpty())
		{
			PlatformApplication::Get()->SetSystemCursor(SystemCursor::Arrow);
		}
		else
		{
			PlatformApplication::Get()->SetSystemCursor(ToSystemCursor(cursorStack.Top()));
		}
	}

	void CApplication::SetFocus(CWidget* widget)
	{
		focusWidget = widget;
	}

	void CApplication::OnSubobjectDetached(Object* object)
	{
		Super::OnSubobjectDetached(object);

		if (!object)
			return;

		if (object->IsOfType<CWindow>())
		{
			windows.Remove((CWindow*)object);
		}
	}

	void CApplication::OnSubobjectAttached(Object* object)
	{
		Super::OnSubobjectAttached(object);

		if (!object)
			return;

		if (object->IsOfType<CWindow>())
		{
			windows.Add((CWindow*)object);
		}
	}

} // namespace CE::Widgets

