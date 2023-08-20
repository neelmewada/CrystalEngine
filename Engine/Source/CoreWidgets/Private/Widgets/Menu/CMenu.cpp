#include "CoreWidgets.h"


namespace CE::Widgets
{

    CMenu::CMenu()
    {

    }

    CMenu::~CMenu()
    {
        
    }

	void CMenu::Construct()
	{
		Super::Construct();
	}

	Vec2 CMenu::CalculateIntrinsicContentSize(f32 width, f32 height)
	{
		return GetComputedLayoutSize();
	}

	void CMenu::OnAfterComputeStyle()
	{
		Super::OnAfterComputeStyle();

	}

	void CMenu::ShowContextMenu()
	{
		Show(Vec2());
	}

	void CMenu::Hide()
	{
		isShown = false;

		for (auto child : attachedWidgets)
		{
			if (child->GetClass()->IsSubclassOf<CMenuItem>())
			{
				CMenuItem* item = (CMenuItem*)child;
				if (item->HasSubMenu() && item->GetSubMenu() != this)
					item->GetSubMenu()->Hide();
			}
		}
	}

    void CMenu::HideAllInChain()
    {
        Hide();
        
        if (parent != nullptr && parent->GetClass()->IsSubclassOf<CMenuItem>())
        {
            CWidget* parentsParent = ((CMenuItem*)parent)->GetOwner();
            if (parentsParent != nullptr && parentsParent->GetClass()->IsSubclassOf<CMenu>())
            {
                ((CMenu*)parentsParent)->HideAllInChain();
            }
        }
    }

	void CMenu::Show(Vec2 pos)
	{
		isShown = true;
		pushShow = true;
		this->showContext = nullptr;
		this->showPos = CalculateShowPosition(pos);

		SetNeedsStyle();
		SetNeedsLayout();
	}

	void CMenu::Show(CWidget* context)
	{
		isShown = true;
		pushShow = true;
		this->showContext = context;
		this->showPos = CalculateShowPosition();

		SetNeedsStyle();
		SetNeedsLayout();
	}

	Vec2 CMenu::CalculateShowPosition(Vec2 tryPosition)
	{
		if (tryPosition == Vec2())
			tryPosition = GUI::GetMousePos();

		SetNeedsLayout();
		UpdateLayoutIfNeeded(); // Calculate layout

		auto menuPos = tryPosition;
		auto menuSize = GetComputedLayoutSize();
		auto screenSize = PlatformApplication::Get()->GetMainScreenSize();
		Vec2 contextWidgetSize = Vec2();

		if (showContext != nullptr) // Context is the widget that is opening this menu. Ex: A button or a parent menu item.
		{
			menuPos = showContext->GetScreenPosition();
			contextWidgetSize = showContext->GetComputedLayoutSize();
			CMenuItem* menuItem = nullptr;
			if (showContext->GetClass()->IsSubclassOf<CMenuItem>() && (menuItem = (CMenuItem*)showContext)->GetOwner() != nullptr &&
				!menuItem->GetOwner()->IsMenuBar())
				contextWidgetSize.y = 0; // We only care about width of menu item
			else
				contextWidgetSize.x = 0;
		}

		menuPos.x += contextWidgetSize.x; // Try opening to the right side of parent menu

		if (menuPos.x + menuSize.x >= screenSize.x - 5)
		{
			menuPos.x = screenSize.x - 5 - menuSize.x - contextWidgetSize.x; // Move to left if no space available to right
		}

		auto verticalPos = menuPosition;
		if (verticalPos == CMenuPosition::Bottom && menuPos.y + contextWidgetSize.y + menuSize.y >= screenSize.y - 40)
			verticalPos = CMenuPosition::Top; // Show at bottom if no space available on top
		else if (verticalPos == CMenuPosition::Top && menuPos.y - contextWidgetSize.y - menuSize.y <= 5)
			verticalPos = CMenuPosition::Bottom; // and vice versa

		if (verticalPos == CMenuPosition::Bottom)
			menuPos.y += contextWidgetSize.y;
		else if (verticalPos == CMenuPosition::Top)
			menuPos.y -= menuSize.y;

		return menuPos;
	}

    void CMenu::OnDrawGUI()
    {
		auto size = GetComputedLayoutSize();

		auto viewportSize = GUI::GetMainViewport()->size;

		if (showPos.x == 0 || showPos.y == 0)
		{
			showPos = CalculateShowPosition();
		}

		if (pushShow)
		{
			pushShow = false;
			GUI::OpenPopup(GetUuid());

			GUI::SetNextWindowPos(showPos);
		}

		int pushedColors = 0;
		int pushedVars = 0;

		if (defaultStyleState.background.a > 0)
		{
			GUI::PushStyleColor(GUI::StyleCol_PopupBg, defaultStyleState.background);
			pushedColors++;
		}

		if (defaultStyleState.borderColor.a > 0)
		{
			GUI::PushStyleColor(GUI::StyleCol_Border, defaultStyleState.borderColor);
			pushedColors++;
		}

		if (defaultStyleState.borderThickness > 0.2f)
		{
			GUI::PushStyleVar(GUI::StyleVar_WindowBorderSize, defaultStyleState.borderThickness);
			pushedVars++;
		}

		GUI::SetNextWindowSize(size);
		
		GUI::WindowFlags flags = GUI::WF_NoMove | GUI::WF_NoResize;
		if (parent != nullptr && parent->GetClass()->IsSubclassOf<CMenuItem>())
		{
			CMenuItem* owner = (CMenuItem*)GetOwner();
			if (!owner->IsInsideMenuBar())
				flags |= GUI::WF_ChildMenu | GUI::WF_ChildWindow;
		}

		bool isOpen = GUI::BeginPopup(GetName().GetString(), GetUuid(), flags);

		if (isOpen)
		{
			if (!isShown)
				GUI::CloseCurrentPopup();

			GUI::PushZeroingChildCoordinateSpace();
			{
				for (auto child : attachedWidgets)
				{
					child->Render();
				}
			}
			GUI::PopChildCoordinateSpace();

			GUI::EndPopup();
            PollEvents();
		}
		else
		{
			if (isShown)
			{
				Hide(); // Hide `this` and all children
			}
			isShown = false;
		}

		if (pushedVars > 0)
		{
			GUI::PopStyleVar(pushedVars);
			pushedVars = 0;
		}

		if (pushedColors > 0)
		{
			GUI::PopStyleColor(pushedColors);
			pushedColors = 0;
		}
    }
    
	bool CMenu::IsAnySubMenuOpen()
	{
		//if (!this->IsShown())
		//	return false;

		for (CWidget* child : attachedWidgets)
		{
			if (child->GetClass()->IsSubclassOf<CMenuItem>())
			{
				CMenuItem* childMenuItem = (CMenuItem*)child;
				if (childMenuItem->HasSubMenu())
				{
					if (childMenuItem->GetSubMenu()->IsShown())
						return true;
					else if (childMenuItem->GetSubMenu()->IsAnySubMenuOpen())
						return true;
				}
			}
		}

		return false;
	}

	void CMenu::UpdateLayoutIfNeeded()
	{
		UpdateStyleIfNeeded();

		YGNodeCalculateLayout(node, YGUndefined, YGUndefined, YGDirectionLTR);

		SetNeedsLayoutRecursive(false);
	}

	bool CMenu::OnSubMenuItemHovered(CMenuItem* subMenuItem)
    {
        bool result = false;
        
        for (CWidget* child : attachedWidgets)
        {
            if (child->GetClass()->IsSubclassOf<CMenuItem>())
            {
                CMenuItem* childMenuItem = (CMenuItem*)child;
                if (childMenuItem == subMenuItem)
                {
                    if (childMenuItem->HasSubMenu() && !childMenuItem->GetSubMenu()->IsShown())
                        childMenuItem->GetSubMenu()->Show(childMenuItem);
                    result = true;
                }
                else
                {
                    if (childMenuItem->HasSubMenu())
                        childMenuItem->GetSubMenu()->Hide();
                }
            }
        }
        
        return result;
    }

    void CMenu::HandleEvent(CEvent* event)
    {
        
        if (event->type == CEventType::MouseButtonClick)
        {
            
        }
        else if (event->type == CEventType::MouseEnter)
        {
            if (IsShown() && event->sender != nullptr && event->sender->GetClass()->IsSubclassOf<CMenuItem>())
            {
                CMenuItem* menuItem = (CMenuItem*)event->sender;
                
                if (OnSubMenuItemHovered(menuItem))
                    event->HandleAndStopPropagation();
            }
        }
        else if (event->type == CEventType::MouseLeave)
        {
			
        }
        
        Super::HandleEvent(event);
    }
    
} // namespace CE

