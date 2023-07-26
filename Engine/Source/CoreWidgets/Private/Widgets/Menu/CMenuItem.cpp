#include "CoreWidgets.h"

namespace CE::Widgets
{
	static CTexture rightArrowIcon{};

    CMenuItem::CMenuItem()
    {
		isInteractable = true;
		AddStyleClass("MenuItem");

		if (!rightArrowIcon.IsValid())
			rightArrowIcon = GetStyleManager()->SearchImageResource("/Icons/right_arrow_32.png");
    }

    CMenuItem::~CMenuItem()
    {
        
    }

	Vec2 CMenuItem::CalculateIntrinsicContentSize(f32 width, f32 height)
	{
		Vec2 checkMark = {};
		if (isToggleable || (HasSubMenu() && !IsInsideMenuBar()))
			checkMark.x += GUI::GetFontSize() * 0.7f;
		f32 iconOffset = 0;
		if (!IsInsideMenuBar())
			iconOffset = 18;

		return Vec2(iconOffset, 0) + GUI::CalculateTextSize(text) + Vec2(15, 5) + checkMark;
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

	void CMenuItem::LoadIcon(const String& iconSearchPath)
	{
		icon = GetStyleManager()->SearchImageResource(iconSearchPath);
	}

	void CMenuItem::OnClicked()
	{

	}

	void CMenuItem::OnDrawGUI()
    {
		auto rect = GetComputedLayoutRect();
		auto padding = GetComputedLayoutPadding();

		f32 centerY = (rect.min.y + rect.max.y) / 2;

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

		f32 iconOffset = 0;
		f32 iconSize = 14;
		if (!IsInsideMenuBar())
			iconOffset = 18;

		GUI::Text(rect + Rect(padding.left + iconOffset, padding.top, -padding.right + iconOffset, -padding.bottom), text, defaultStyleState);

		if (icon.IsValid())
		{
			Rect iconRect = Rect(rect.left + padding.left, centerY - iconSize / 2,
				rect.left + padding.left + iconSize, centerY + iconSize / 2);
			GUI::Image(iconRect, icon.id, {});
		}

		if (HasSubMenu() && !IsInsideMenuBar() && rightArrowIcon.IsValid())
		{
			const f32 toggleSize = GUI::GetFontSize() * 0.7f;
			f32 togglePosX = rect.max.x - padding.right - toggleSize;
			f32 togglePosY = (rect.min.y + rect.max.y) / 2 - toggleSize / 2;
			Rect toggleRect = Rect(togglePosX, togglePosY, togglePosX + toggleSize, togglePosY + toggleSize);

			//GUI::FillRect(toggleRect, defaultStyleState.foreground);
			GUI::Image(toggleRect, rightArrowIcon.id, {});
		}
		else if (IsToggleable() && IsToggled())
		{
			const f32 toggleSize = GUI::GetFontSize() * 0.7f;
			f32 togglePosX = rect.max.x - padding.right - toggleSize;
			f32 togglePosY = (rect.min.y + rect.max.y) / 2 - toggleSize / 2;
			Rect toggleRect = GUI::WindowRectToGlobalRect(Rect(togglePosX, togglePosY, togglePosX + toggleSize, togglePosY + toggleSize));

			GUI::FillCheckMark(Vec2(toggleRect.x, toggleRect.y), defaultStyleState.foreground, toggleSize);
		}
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
				if (IsToggleable())
				{
					toggleValue = !toggleValue;

					OnClicked();
					emit OnMenuItemClicked(this);
				}
				else
				{
					((CMenu*)parent)->HideAllInChain();

					OnClicked();
					emit OnMenuItemClicked(this);
				}
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

