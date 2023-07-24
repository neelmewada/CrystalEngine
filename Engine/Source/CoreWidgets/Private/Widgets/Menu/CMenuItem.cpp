#include "CoreWidgets.h"

namespace CE::Widgets
{

    CMenuItem::CMenuItem()
    {
		isInteractable = true;
		AddStyleClass("MenuItem");
    }

    CMenuItem::~CMenuItem()
    {
        
    }

	Vec2 CMenuItem::CalculateIntrinsicContentSize(f32 width, f32 height)
	{
		return GUI::CalculateTextSize(text) + Vec2(10, 5);
	}

	void CMenuItem::Construct()
	{
		Super::Construct();

		if (GetOwner() != nullptr && GetOwner()->GetClass()->IsSubclassOf<CMenuBar>())
		{
			AddStyleClass("MenuBarItem");
		}
	}

	bool CMenuItem::HasSubMenu()
	{
		return subMenu != nullptr;
	}

	bool CMenuItem::IsInsideMenuBar()
	{
		return GetOwner() != nullptr && GetOwner()->GetClass()->IsSubclassOf<CMenuBar>();
	}

	void CMenuItem::OnDrawGUI()
    {
		auto rect = GetComputedLayoutRect();
		auto padding = GetComputedLayoutPadding();

		DrawDefaultBackground();

		GUI::InvisibleButton(rect, GetUuid());
		PollEvents();
        
        if (HasSubMenu() && subMenu->IsShown())
        {
            stateFlags = CStateFlag::Hovered; // Force hovered state when in sub-menu and disable interactivity
			isInteractable = false;
        }
		else
		{
			isInteractable = true;
		}

		GUI::Text(rect + Rect(padding.left, padding.top, -padding.right, -padding.bottom), text, defaultStyleState);
    }

	void CMenuItem::HandleEvent(CEvent* event)
	{
        
		if (event->type == CEventType::MouseButtonClick)
		{
			if (HasSubMenu())
			{
				subMenu->Show(this);
			}
			else if (parent != nullptr && parent->GetClass()->IsSubclassOf<CMenu>())
			{
				((CMenu*)parent)->HideAllInChain();
			}

			event->HandleAndStopPropagation();
		}
		else if (event->type == CEventType::MouseEnter)
		{
			
		}
		else if (event->type == CEventType::MouseLeave)
		{
            
		}

		Super::HandleEvent(event);
	}
    
	CMenuItemHeader::CMenuItemHeader()
	{
		isInteractable = false;
		AddStyleClass("MenuItem");
	}

	CMenuItemHeader::~CMenuItemHeader()
	{

	}

	Vec2 CMenuItemHeader::CalculateIntrinsicContentSize(f32 width, f32 height)
	{
		return GUI::CalculateTextSize(title) + Vec2(20, 4);
	}

	void CMenuItemHeader::OnDrawGUI()
	{
		auto rect = GetComputedLayoutRect();
		auto padding = GetComputedLayoutPadding();

		rect.min.x += padding.left;
		
		defaultStyleState.textAlign = GUI::TextAlign_MiddleLeft;
		auto textSize = GUI::CalculateTextSize(title) + Vec2(10, 0);

		GUI::Text(Rect(rect.min, rect.min + Vec2(textSize.x, rect.max.y - rect.min.y)), title, defaultStyleState);

		f32 centerY = (rect.min.y + rect.max.y) / 2.0f;
		f32 lineStartX = rect.min.x + textSize.x + 1;
		f32 lineEndX = rect.max.x - padding.right;
		f32 lineThickness = 2.0f;

		if (lineStartX + 5 > lineEndX)
			return;

		Rect lineRect{ lineStartX, centerY - lineThickness / 2.0f, lineEndX, centerY + lineThickness / 2.0f };
		FillRect(defaultStyleState.foreground, lineRect);
	}

} // namespace CE

