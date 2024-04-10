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
		scheduler = initInfo.scheduler;
		numFramesInFlight = initInfo.numFramesInFlight;
		resourceLoader = initInfo.resourceLoader;

		CE_ASSERT(resourceLoader != nullptr, "CApplication passed with null resource loader!");

		registeredFonts[defaultFontName] = initInfo.defaultFont;

		PlatformApplication::Get()->AddMessageHandler(this);
	}

	void CApplication::Shutdown()
	{
		for (int i = destructionQueue.GetSize() - 1; i >= 0; --i)
		{
			destructionQueue[i]->Destroy();
			destructionQueue.RemoveAt(i);
		}

		for (int i = platformWindows.GetSize() - 1; i >= 0; --i)
		{
			delete platformWindows[i];
		}
		platformWindows.Clear();

		PlatformApplication::Get()->RemoveMessageHandler(this);
	}

	void CApplication::Tick()
	{
		constexpr u32 destroyAfterFrames = Limits::MaxSwapChainImageCount;

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

		Enum* keyCodeEnum = GetStaticEnum<KeyCode>();
		Enum* keyModifierEnum = GetStaticEnum<KeyModifier>();

		keyModifierStates = KeyModifier::None;
		keyPressStates.Resize(keyCodeEnum->GetConstantsCount());

		for (int i = 0; i < keyModifierEnum->GetConstantsCount(); ++i)
		{
			if (InputManager::TestModifiers((KeyModifier)keyModifierEnum->GetConstant(i)->GetValue()))
			{
				keyModifierStates |= (KeyModifier)keyModifierEnum->GetConstant(i)->GetValue();
			}
		}

		std::function<CWidget* (CWidget*)> getBottomMostHoveredWidget = [&](CWidget* widget) -> CWidget*
			{
				if (widget == nullptr || !widget->IsEnabled() || !widget->interactable)
					return nullptr;

				Rect widgetRect = widget->GetScreenSpaceRect();

				CPlatformWindow* nativeWindow = widget->GetNativeWindow();
				if (nativeWindow)
				{
					if (widget->IsOfType<CDockSpace>() && widget->parent == nullptr)
					{
						//CE_LOG(Info, All, "{} | {} | {}", nativeWindow->IsFocused(), nativeWindow->IsShown(), nativeWindow->IsMinimized());
					}

					if (!nativeWindow->IsFocused() || !nativeWindow->IsShown() || nativeWindow->IsMinimized())
					{
						return nullptr;
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

		for (int i = 0; i < platformWindows.GetSize(); ++i)
		{
			CWidget* curHoveredWidget = getBottomMostHoveredWidget(platformWindows[i]->owner);
			if (curHoveredWidget != nullptr)
			{
				hoveredWidget = curHoveredWidget;
			}
		}

		if (hoveredWidget)
		{
			hoveredWindow = hoveredWidget->ownerWindow;
		}

		if (hoveredWidget && hoveredWidget->GetNativeWindow())
		{
			PlatformWindow* platformWindow = hoveredWidget->GetNativeWindow()->platformWindow;
			if (platformWindow && !platformWindow->IsFocused())
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
			mouseEvent.keyModifiers = keyModifierStates;

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
			mouseEvent.keyModifiers = keyModifierStates;

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
			mouseEvent.keyModifiers = keyModifierStates;

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
			mouseEvent.keyModifiers = keyModifierStates;

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
				dragEvent.keyModifiers = keyModifierStates;

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
				event.keyModifiers = keyModifierStates;

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
						dragEvent.keyModifiers = keyModifierStates;

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
				event.keyModifiers = keyModifierStates;

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
					dragEvent.keyModifiers = keyModifierStates;

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

		CWidget* keyEventWidget = curFocusedWidget;
		while (keyEventWidget != nullptr && !keyEventWidget->receiveKeyEvents)
		{
			keyEventWidget = keyEventWidget->parent;
		}

		if (keyEventWidget)
		{
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

		for (int i = 0; i < platformWindows.GetSize(); i++)
		{
			platformWindows[i]->Tick();
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
		
		for (auto window : platformWindows)
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
		for (int i = 0; i < platformWindows.GetSize(); ++i)
		{
			PlatformWindow* platformWindow = platformWindows[i]->platformWindow;

			if (!platformWindow)
				continue;

			FrameAttachmentDatabase& attachmentDatabase = scheduler->GetAttachmentDatabase();

			Name id = String::Format("{}", platformWindow->GetWindowId());

			attachmentDatabase.EmplaceFrameAttachment(id, platformWindows[i]->swapChain);

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

					scheduler->PresentSwapChain(platformWindows[i]->swapChain);
				}
				scheduler->EndScope();
			}
		}
	}

	void CApplication::SetDrawListMasks(RHI::DrawListMask& outMask)
	{
		for (int i = 0; i < platformWindows.GetSize(); ++i)
		{
			outMask.Set(platformWindows[i]->drawListTag);
		}
	}

	void CApplication::FlushDrawPackets(RHI::DrawListContext& drawList, u32 imageIndex)
	{
		for (int i = 0; i < platformWindows.GetSize(); ++i)
		{
			if (!platformWindows[i]->owner->IsVisible() || !platformWindows[i]->owner->IsEnabled())
				continue;

			const auto& packets = platformWindows[i]->FlushDrawPackets(imageIndex);

			for (RHI::DrawPacket* drawPacket : packets)
			{
				drawList.AddDrawPacket(drawPacket);
			}
		}
	}

	void CApplication::SubmitDrawPackets(RHI::DrawListContext& drawList)
	{
		for (int i = 0; i < platformWindows.GetSize(); ++i)
		{
			PlatformWindow* platformWindow = platformWindows[i]->platformWindow;

			if (!platformWindow)
				continue;

			Name id = String::Format("{}", platformWindow->GetWindowId());

			scheduler->SetScopeDrawList(id, &drawList.GetDrawListForTag(platformWindows[i]->drawListTag));
		}
	}

	void CApplication::OnWindowDestroyed(PlatformWindow* nativeWindow)
	{
		for (int i = platformWindows.GetSize() - 1; i >= 0; --i)
		{
			if (platformWindows[i]->platformWindow == nativeWindow)
			{
				platformWindows[i]->owner->nativeWindow = nullptr; // Clear the nativeWindow pointer
				platformWindows[i]->owner->Destroy();
				if (!platformWindows[i]->isDeleted)
				{
					platformWindows[i]->platformWindow = nullptr;
					delete platformWindows[i];
				}
				else
				{
					platformWindows.RemoveAt(i);
				}
				break;
			}
		}
	}

	void CApplication::OnWindowClosed(PlatformWindow* nativeWindow)
	{
		for (int i = platformWindows.GetSize() - 1; i >= 0; --i)
		{
			if (platformWindows[i]->platformWindow == nativeWindow)
			{
				platformWindows.RemoveAt(i);
				break;
			}
		}
	}

	void CApplication::OnWindowCreated(PlatformWindow* window)
	{

	}

	RPI::Texture* CApplication::LoadImage(const Name& assetpath)
	{
		if (texturesByPath.KeyExists(assetpath))
		{
			return texturesByPath[assetpath];
		}

		RPI::Texture* texture = resourceLoader->LoadImage(assetpath);
		texturesByPath[assetpath] = texture;
		return texture;
	}

	Vec2 CApplication::CalculateTextSize(const String& text, f32 fontSize, Name fontName, f32 width)
	{
		for (CPlatformWindow* window : platformWindows)
		{
			if (window->renderer != nullptr)
			{
				return window->renderer->CalculateTextSize(text, fontSize, fontName, width);
			}
		}

		return Vec2();
	}

	Vec2 CApplication::CalculateTextOffsets(Array<Rect>& outOffsetRects, const String& text, f32 fontSize,
		Name fontName, f32 width)
	{
		for (CPlatformWindow* window : platformWindows)
		{
			if (window->renderer != nullptr)
			{
				return window->renderer->CalculateTextOffsets(outOffsetRects, text, fontSize, fontName, width);
			}
		}

		return Vec2();
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
		for (CPlatformWindow* window : platformWindows)
		{
			if (window->platformWindow == nativeWindow)
			{
				window->owner->SetNeedsStyle();
				window->owner->SetNeedsLayout();
				window->owner->SetNeedsPaint();
			}
		}
	}

	void CApplication::OnWindowRestored(PlatformWindow* nativeWindow)
	{
		for (CPlatformWindow* window : platformWindows)
		{
			if (window->platformWindow == nativeWindow)
			{
				window->owner->SetNeedsStyle();
				window->owner->SetNeedsLayout();
				window->owner->SetNeedsPaint();
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
			platformWindows.Remove(((CWindow*)object)->nativeWindow);
		}
	}

	void CApplication::OnSubobjectAttached(Object* object)
	{
		Super::OnSubobjectAttached(object);

		if (!object)
			return;

		if (object->IsOfType<CWindow>())
		{
			if (((CWindow*)object)->nativeWindow != nullptr)
				platformWindows.Add(((CWindow*)object)->nativeWindow);
		}
	}

} // namespace CE::Widgets

