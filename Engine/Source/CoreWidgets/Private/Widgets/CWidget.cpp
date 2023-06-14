
#include "CoreWidgets.h"

namespace CE::Widgets
{

	CWidget::CWidget()
	{

	}

	void CWidget::ConstructInternal()
	{
		style.ApplyStyle(gStyleManager->globalStyle);

		for (const auto& styleGroup : gStyleManager->styleGroups)
		{
			if (styleGroup.selector.TestSelector(this))
			{
				style.ApplyStyle(styleGroup.style);
			}
		}
	}

	void CWidget::UpdateStyle()
	{
		style.SetDirty(true);
	}

	void CWidget::OnAttachedTo(CWidget* parent)
	{
		if (inheritedPropertiesInitialized) // Inherited properties can only be initialized once
			return;

		for (auto& [property, array] : style.styleMap)
		{
			bool foundDefaultState = false;
			for (auto& value : array)
			{
				if (value.state == CStateFlag::Default)
					foundDefaultState = true;
				if (value.enumValue == CStyleValue::Inherited && value.valueType == CStyleValue::Type_Enum)
				{
					auto& parentArray = parent->style.styleMap[property];
					for (const auto& parentValue : parentArray)
					{
						if (parentValue.state == CStateFlag::Default)
						{
							value = parentValue;
							break;
						}
					}
				}
			}

			if (!foundDefaultState && CStyle::IsInheritedProperty(property)) // If property is auto-inheritable
			{
				auto& parentArray = parent->style.styleMap[property];
				for (const auto& parentValue : parentArray)
				{
					if (parentValue.state == CStateFlag::Default)
					{
						array.Add(parentValue);
						break;
					}
				}
			}
		}

		inheritedPropertiesInitialized = true;
	}

	void CWidget::OnDetachedFrom(CWidget* parent)
	{

	}

	void CWidget::OnSubobjectAttached(Object* subobject)
	{
		if (subobject == nullptr)
			return;
		if (subobject->GetClass()->IsSubclassOf<CWidget>())
		{
			CWidget* subWidget = (CWidget*)subobject;
			attachedWidgets.Add(subWidget);
			OnSubWidgetAttached(subWidget);
			subWidget->OnAttachedTo(this);
		}
	}

	void CWidget::OnSubobjectDetached(Object* subobject)
	{
		if (subobject == nullptr)
			return;
		if (subobject->GetClass()->IsSubclassOf<CWidget>())
		{
			CWidget* subWidget = (CWidget*)subobject;
			attachedWidgets.Remove(subWidget);
			OnSubWidgetDetached(subWidget);
			subWidget->OnDetachedFrom(this);
		}
	}
    
	CWidget::~CWidget()
	{
		
	}


	void CWidget::PollEvents()
	{
		CWindow* thisWindow = nullptr;
		if (IsWindow())
			thisWindow = (CWindow*)this;

		bool focused = IsWindow() ? GUI::IsWindowFocused(thisWindow->GetTitle(), GUI::FOCUS_ChildWindows | GUI::FOCUS_DockHierarchy) : GUI::IsItemFocused();

		if (focused != isFocused) // Focus changed
		{
			CFocusEvent event{};
			event.name = "FocusChanged";
			event.type = CEventType::FocusChanged;
			event.gotFocus = focused;
			event.sender = this;
			HandleEvent(&event);

			isFocused = focused;
		}

		if (!IsWindow())
		{
			bool hovered = GUI::IsItemHovered();

			// Mouse: Press, Release

			b8 leftMouseDown = GUI::IsMouseDown(GUI::MouseButton::Left);

			if (leftMouseDown != prevLeftMouseDown && hovered) // Left mouse state changed within bounds
			{
				isLeftMousePressedInside = leftMouseDown && hovered;

				if (isLeftMousePressedInside) // Press inside
				{
					CMouseEvent event{};
					event.name = "MousePress";
					event.type = CEventType::MousePress;
					event.mousePos = GUI::GetMousePos();
					event.mouseButton = 0; // Left = 0
					event.prevMousePos = prevHoverPos;
					event.isInside = true;
					event.sender = this;

					HandleEvent(&event);
					prevHoverPos = event.mousePos;
				}
				else // Release inside
				{
					CMouseEvent event{};
					event.name = "MouseRelease";
					event.type = CEventType::MouseRelease;
					event.mousePos = GUI::GetMousePos();
					event.mouseButton = 0; // Left = 0
					event.prevMousePos = prevHoverPos;
					event.isInside = true;
					event.sender = this;

					HandleEvent(&event);
					prevHoverPos = event.mousePos;
				}
			}
			else if (isLeftMousePressedInside && !leftMouseDown && !hovered) // Release outside
			{
				isLeftMousePressedInside = false;

				CMouseEvent event{};
				event.name = "MouseRelease";
				event.type = CEventType::MouseRelease;
				event.mousePos = GUI::GetMousePos();
				event.mouseButton = 0; // Left = 0
				event.prevMousePos = prevHoverPos;
				event.isInside = false;
				event.sender = this;

				HandleEvent(&event);
				prevHoverPos = event.mousePos;
			}

			prevLeftMouseDown = GUI::IsMouseDown(GUI::MouseButton::Left);

			// Mouse: Enter, Move, Leave
			
			if (hovered && !isHovered)
			{
				CMouseEvent event{};
				event.name = "MouseEnter";
				event.type = CEventType::MouseEnter;
				event.mousePos = GUI::GetMousePos();
				event.sender = this;
				HandleEvent(&event);
				isHovered = hovered;
				prevHoverPos = event.mousePos;
			}
			else if (hovered && isHovered)
			{
				auto mousePos = GUI::GetMousePos();
				if (mousePos.x != prevHoverPos.x && mousePos.y != prevHoverPos.y)
				{
					CMouseEvent event{};
					event.name = "MouseMove";
					event.type = CEventType::MouseMove;
					event.mousePos = GUI::GetMousePos();
					event.prevMousePos = prevHoverPos;
					event.sender = this;

					HandleEvent(&event);
					prevHoverPos = mousePos;
				}
			}
			else if (!hovered && isHovered)
			{
				CMouseEvent event{};
				event.name = "MouseLeave";
				event.type = CEventType::MouseLeave;
				event.mousePos = GUI::GetMousePos();
				event.prevMousePos = prevHoverPos;
				event.sender = this;
				HandleEvent(&event);
				isHovered = hovered;
				prevHoverPos = event.mousePos;
			}

			// Mouse Click:

			bool isLeftClicked = GUI::IsItemClicked(GUI::MouseButton::Left);
			bool isRightClicked = GUI::IsItemClicked(GUI::MouseButton::Right);
			bool isMiddleClicked = GUI::IsItemClicked(GUI::MouseButton::Middle);

			if (isLeftClicked || isRightClicked || isMiddleClicked)
			{
				if (GUI::IsItemHovered())
				{
					CMouseEvent event{};
					event.name = "MouseClick";
					event.type = CEventType::MouseButtonClick;
					event.mouseButton = isLeftClicked ? 0 : (isRightClicked ? 1 : 2);
					event.mousePos = GUI::GetMousePos();
					event.prevMousePos = event.mousePos;
					event.sender = this;
					HandleEvent(&event);
				}
			}

			bool isLeftDblClicked = GUI::IsItemDoubleClicked(GUI::MouseButton::Left);
			bool isRightDblClicked = GUI::IsItemDoubleClicked(GUI::MouseButton::Right);
			bool isMiddleDblClicked = GUI::IsItemDoubleClicked(GUI::MouseButton::Middle);

			if (isLeftDblClicked || isRightDblClicked || isMiddleDblClicked)
			{
				if (GUI::IsItemHovered())
				{
					CMouseEvent event{};
					event.name = "MouseDoubleClick";
					event.type = CEventType::MouseButtonDoubleClick;
					event.mouseButton = isLeftDblClicked ? 0 : (isRightDblClicked ? 1 : 2);
					event.mousePos = GUI::GetMousePos();
					event.prevMousePos = event.mousePos;
					event.sender = this;
					HandleEvent(&event);
				}
			}
		}
		else // Window
		{
			thisWindow = (CWindow*)this;

			// Mouse Click:

			bool isLeftClicked = GUI::IsMouseClicked(GUI::MouseButton::Left);
			bool isRightClicked = GUI::IsMouseClicked(GUI::MouseButton::Right);
			bool isMiddleClicked = GUI::IsMouseClicked(GUI::MouseButton::Middle);
			
			if (isLeftClicked || isRightClicked || isMiddleClicked)
			{
				bool isWindowHovered = GUI::IsWindowHovered(thisWindow->GetTitle(), GUI::Hovered_ChildWindows | GUI::Hovered_DockHierarchy | GUI::Hovered_NoPopupHierarchy);
				if (isWindowHovered)
				{
					CMouseEvent event{};
					event.name = "MouseClick";
					event.type = CEventType::MouseButtonClick;
					event.mouseButton = isLeftClicked ? 0 : (isRightClicked ? 1 : 2);
					event.mousePos = GUI::GetMousePos();
					event.prevMousePos = event.mousePos;
					event.sender = this;
					HandleEvent(&event);
				}
			}

			bool isLeftDblClicked = GUI::IsMouseDoubleClicked(GUI::MouseButton::Left);
			bool isRightDblClicked = GUI::IsMouseDoubleClicked(GUI::MouseButton::Right);
			bool isMiddleDblClicked = GUI::IsMouseDoubleClicked(GUI::MouseButton::Middle);

			if (isLeftDblClicked || isRightDblClicked || isMiddleDblClicked)
			{
				bool isWindowHovered = GUI::IsWindowHovered(thisWindow->GetTitle(), GUI::Hovered_ChildWindows | GUI::Hovered_DockHierarchy | GUI::Hovered_NoPopupHierarchy);
				if (isWindowHovered)
				{
					CMouseEvent event{};
					event.name = "MouseDoubleClick";
					event.type = CEventType::MouseButtonDoubleClick;
					event.mouseButton = isLeftClicked ? 0 : (isRightClicked ? 1 : 2);
					event.mousePos = GUI::GetMousePos();
					event.prevMousePos = event.mousePos;
					event.sender = this;
					HandleEvent(&event);
				}
			}
		}
	}

	void CWidget::RenderGUI()
	{
		OnDrawGUI();
    }

    void CWidget::SetWidgetFlags(WidgetFlags flags)
    {
        this->widgetFlags = flags;
    }

    WidgetFlags CWidget::GetWidgetFlags() const
    {
        return widgetFlags;
    }
	
	CWidget* CWidget::GetOwner()
	{
		if (GetOuter() == nullptr)
			return nullptr;
		return DynamicCast<CWidget>(GetOuter());
	}

	CWindow* CWidget::GetOwnerWindow()
	{
		auto owner = GetOwner();
		while (owner != nullptr)
		{
			if (owner->IsWindow())
				return (CWindow*)owner;
			owner = owner->GetOwner();
		}
		return nullptr;
	}

	CWidget* CWidget::FindSubWidget(const Name& name)
	{
		for (auto subWidget : attachedWidgets)
		{
			if (subWidget != nullptr && subWidget->GetName() == name)
				return subWidget;
		}
		return nullptr;
	}

	void CWidget::HandleEvent(CEvent* event)
    {
		if (event->isHandled && event->stopPropagation)
			return;

		if (event->GetEventType() == CEventType::MouseButtonClick)
		{
			CMouseEvent* mouseEvent = (CMouseEvent*)event;
			fire OnMouseClick(mouseEvent);
		}

		for (CWidget* parent = GetOwner(); parent != nullptr; parent = parent->GetOwner())
		{
			if (event->isHandled && event->stopPropagation)
				return;
			if (parent != this)
				parent->HandleEvent(event);
		}
    }

} // namespace CE::Widgets

