#include "FusionCore.h"

namespace CE
{

    FRootContext::FRootContext()
    {

    }

    FRootContext::~FRootContext()
    {
        
    }

    void FRootContext::TickInput()
    {
        ZoneScoped;

	    Super::TickInput();

        for (FFusionContext* childContext : childContexts)
        {
            if (!childContext->IsOfType<FNativeContext>())
                continue;

            TickNativeContextInput(static_cast<FNativeContext*>(childContext));
        }
    }

	void FRootContext::TickNativeContextInput(FNativeContext* nativeContext)
	{
		ZoneScoped;

		if (nativeContext == nullptr || !IsRootContext())
			return;

		PlatformWindow* platformWindow = nativeContext->platformWindow;
		FWidget* owningWidget = nativeContext->owningWidget;

		if (!owningWidget || FusionApplication::Get()->isExposed || !platformWindow->IsShown())
		{
			return;
		}

		// - Event Handling -

		Vec2 mousePos = nativeContext->ScreenToGlobalSpacePosition(InputManager::GetGlobalMousePosition().ToVec2());
		Vec2 mouseDelta = InputManager::GetMouseDelta().ToVec2();
		Vec2 wheelDelta = InputManager::GetMouseWheelDelta();
		if (prevMousePos.GetSqrMagnitude() == 0)
			prevMousePos = mousePos;

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

		FWidget* hoveredWidget = nativeContext->HitTest(mousePos);

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

		// - Mouse Wheel Events -

		if (abs(wheelDelta.x) >= FLT_EPSILON || abs(wheelDelta.y) >= FLT_EPSILON)
		{
			FMouseEvent mouseEvent{};
			mouseEvent.type = FEventType::MouseWheel;
			mouseEvent.buttons = curButtonMask;
			mouseEvent.mousePosition = mousePos;
			mouseEvent.prevMousePosition = prevMousePos;
			mouseEvent.direction = FEventDirection::BottomToTop;
			mouseEvent.wheelDelta = wheelDelta;
			mouseEvent.keyModifiers = keyModifierStates;

			if (hoveredWidgetStack.NonEmpty())
			{
				FWidget* sender = hoveredWidgetStack.Top();

				while (sender != nullptr && !sender->CapturesMouseWheel())
				{
					sender = sender->parent;
				}

				if (sender)
				{
					mouseEvent.sender = sender;
					sender->HandleEvent(&mouseEvent);
				}
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

				dragEvent.sender = draggedWidget;
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

		if (InputManager::IsMouseButtonDown(MouseButton::Left))
		{
			Array<FPopup*> popupsToClose{};

			for (int i = localPopupStack.GetSize() - 1; i >= 0; --i)
			{
				if (!localPopupStack[i]->AutoClose())
					continue;

				if (hoveredWidget == nullptr || !hoveredWidget->ParentExistsRecursive(localPopupStack[i]))
				{
					popupsToClose.Add(localPopupStack[i]);
				}
			}

			for (FPopup* popup : popupsToClose)
			{
				ClosePopup(popup);
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
						if (dragEventWidget->SupportsDragEvents())
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

				if (mouseButton == MouseButton::Left && draggedWidget != nullptr)
				{
					FDragEvent dragEvent{};
					dragEvent.type = FEventType::DragEnd;
					dragEvent.mousePosition = mousePos;
					dragEvent.prevMousePosition = prevMousePos;
					dragEvent.buttons = (MouseButtonMask)BIT((int)mouseButton);
					dragEvent.keyModifiers = keyModifierStates;

					dragEvent.sender = draggedWidget;
					dragEvent.draggedWidget = draggedWidget;

					dragEvent.sender->HandleEvent(&dragEvent);

					draggedWidget = nullptr;
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

			curFocusWidget = nullptr;
			widgetToFocus = nullptr;
		}

		if (curFocusWidget != nullptr && !curFocusWidget->IsFocused() && curFocusWidget == widgetToFocus)
		{
			curFocusWidget = nullptr;
			widgetToFocus = nullptr;
		}

		if (widgetToFocus != curFocusWidget)
		{
			if (curFocusWidget != nullptr && (widgetToFocus == nullptr || !widgetToFocus->FocusParentExistsRecursive(curFocusWidget)))
			{
				FWidget* base = curFocusWidget;

				while (base != nullptr)
				{
					if (base->parent != nullptr && widgetToFocus->FocusParentExistsRecursive(base->parent))
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
					focusEvent.focusedWidget = widgetToFocus;
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

		prevMousePos = mousePos;
	}
    
} // namespace CE

