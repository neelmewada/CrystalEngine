
#include "CoreWidgets.h"

namespace CE::Widgets
{

	CWidget::CWidget()
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
			OnWidgetAttached(subWidget);
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
			OnWidgetDetached(subWidget);
			subWidget->OnDetachedFrom(this);
		}
	}
    
	CWidget::~CWidget()
	{
        
	}

    void CWidget::BuildWidget()
    {
        if (isBuilt)
            return;
        
        Build();
        isBuilt = true;
    }

    void CWidget::BeginStyle()
    {
        for (const auto& [variable, value] : style.styles)
        {
            auto styleVars = CStyle::GetStyleVar(variable);
            for (GUI::StyleVar styleVar : styleVars)
            {
                if (styleVar != GUI::StyleVar_COUNT)
                {
                    if (GUI::IsStyleVarOfVectorType(styleVar))
                        GUI::PushStyleVar(styleVar, (Vec2)value.vector);
                    else
                        GUI::PushStyleVar(styleVar, value.single);
                    pushedVars++;
                }
            }
            auto styleColors = CStyle::GetStyleColorVar(variable);
            for (GUI::StyleCol styleCol : styleColors)
            {
                if (styleCol != GUI::StyleCol_COUNT)
                {
                    GUI::PushStyleColor(styleCol, value.color);
                    pushedColors++;
                }
            }
        }
    }

    void CWidget::EndStyle()
    {
        if (pushedVars > 0)
            GUI::PopStyleVar(pushedVars);
        if (pushedColors > 0)
            GUI::PopStyleColor(pushedColors);
        
        pushedVars = pushedColors = 0;
    }

	void CWidget::PollEvents()
	{
		bool focused = IsWindow() ? GUI::IsWindowFocused(GUI::FOCUS_ChildWindows | GUI::FOCUS_DockHierarchy) : GUI::IsItemFocused();

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

			b8 leftMouse = GUI::IsMouseDown(GUI::MouseButton::Left) && hovered;

			if (isLeftMouseDown != leftMouse)
			{
				if (leftMouse) // Press
				{
					CMouseEvent event{};
					event.name = "MousePress";
					event.type = CEventType::MousePress;
					event.mousePos = GUI::GetMousePos();
					event.mouseButton = 0; // Left = 0
					event.prevMousePos = prevHoverPos;
					event.sender = this;

					HandleEvent(&event);
					prevHoverPos = event.mousePos;
				}
				else // Release
				{
					CMouseEvent event{};
					event.name = "MouseRelease";
					event.type = CEventType::MouseRelease;
					event.mousePos = GUI::GetMousePos();
					event.mouseButton = 0; // Left = 0
					event.prevMousePos = prevHoverPos;
					event.sender = this;

					HandleEvent(&event);
					prevHoverPos = event.mousePos;
				}

				isLeftMouseDown = leftMouse;
			}

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
					OnMouseClicked(event.mouseButton);
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
					OnMouseDoubleClicked(event.mouseButton);
				}
			}
		}
		else // Window
		{
			// Mouse Click:

			bool isLeftClicked = GUI::IsMouseClicked(GUI::MouseButton::Left);
			bool isRightClicked = GUI::IsMouseClicked(GUI::MouseButton::Right);
			bool isMiddleClicked = GUI::IsMouseClicked(GUI::MouseButton::Middle);
			
			if (isLeftClicked || isRightClicked || isMiddleClicked)
			{
				bool isWindowHovered = GUI::IsWindowHovered(GUI::Hovered_AnyWindow);
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
					OnMouseClicked(event.mouseButton);
				}
			}

			bool isLeftDblClicked = GUI::IsMouseDoubleClicked(GUI::MouseButton::Left);
			bool isRightDblClicked = GUI::IsMouseDoubleClicked(GUI::MouseButton::Right);
			bool isMiddleDblClicked = GUI::IsMouseDoubleClicked(GUI::MouseButton::Middle);

			if (isLeftDblClicked || isRightDblClicked || isMiddleDblClicked)
			{
				bool isWindowHovered = GUI::IsWindowHovered(GUI::Hovered_AnyWindow);
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
					OnMouseDoubleClicked(event.mouseButton);
				}
			}
		}
	}

	void CWidget::RenderGUI()
	{
        BeginStyle();
		OnDrawGUI();
        EndStyle();
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

	void CWidget::HandleEvent(CEvent* event)
    {
		if (event->isHandled && event->stopPropagation)
			return;

		for (CWidget* parent = GetOwner(); parent != nullptr; parent = parent->GetOwner())
		{
			if (event->isHandled && event->stopPropagation)
				return;
			if (parent != this)
				parent->HandleEvent(event);
		}
    }

} // namespace CE::Widgets

