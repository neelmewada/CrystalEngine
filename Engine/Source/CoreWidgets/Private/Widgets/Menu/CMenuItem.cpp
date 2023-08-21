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
		f32 offsetX = 0;
		siblingExistsWithRadioOrToggle = !IsInsideMenuBar() && HasAnySiblingWithToggleOrRadio();

		if (HasSubMenu() && !IsInsideMenuBar())
			offsetX += GUI::GetFontSize() * 0.7f; // Sub-Menu Arrow icon on right side
		if (HasIcon() && !IsInsideMenuBar())
			offsetX += 18; // Menu item icon
		if (siblingExistsWithRadioOrToggle)
			offsetX += 24; // Toggle or radio item

		return Vec2(offsetX, 0) + GUI::CalculateTextSize(text) + Vec2(15, 8);
	}

	void CMenuItem::Construct()
	{
		Super::Construct();

		if (IsInsideMenuBar())
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
		return GetOwner() != nullptr && GetOwner()->IsOfType<CMenuBar>();
	}

	void CMenuItem::LoadIcon(const String& iconSearchPath)
	{
		icon = GetStyleManager()->SearchImageResource(iconSearchPath);
	}

	bool CMenuItem::HasAnySiblingWithToggleOrRadio()
	{
		if (GetOwner() == nullptr)
			return false;
		if (!GetOwner()->GetClass()->IsSubclassOf<CMenu>())
			return false;
		CMenu* parent = (CMenu*)GetOwner();

		for (int i = 0; i < parent->GetSubWidgetCount(); i++)
		{
			auto subWidget = parent->GetSubWidgetAt(i);
			if (subWidget == nullptr || !subWidget->GetClass()->IsSubclassOf<CMenuItem>())
				continue;
			CMenuItem* item = (CMenuItem*)subWidget;
			if (item->IsToggle() || item->IsRadio())
				return true;
		}

		return false;
	}

	void CMenuItem::SetAsDefaultType()
	{
		itemType = ItemType::Default; 
		SetNeedsStyle(); 
		SetNeedsLayout();
	}

	void CMenuItem::SetAsToggle()
	{
		itemType = ItemType::Toggle;
		SetNeedsStyle();
		SetNeedsLayout();
	}

	void CMenuItem::SetAsRadio(const String& radioGroupName)
	{
		this->radioGroupName = radioGroupName;
		itemType = ItemType::Radio;
		SetNeedsStyle();
		SetNeedsLayout();
	}

	void CMenuItem::SetToggleValue(bool value)
	{
		if (IsToggle()) 
			isToggled = value;
	}

	void CMenuItem::SetRadioValue(bool value)
	{
		if (!IsRadio())
			return;

		if (value && GetOwner() != nullptr && GetOwner()->GetClass()->IsSubclassOf<CMenu>())
		{
			CMenu* parent = (CMenu*)GetOwner();
			
			for (int i = 0; i < parent->GetSubWidgetCount(); i++)
			{
				auto subWidget = parent->GetSubWidgetAt(i);
				if (subWidget == nullptr || !subWidget->GetClass()->IsSubclassOf<CMenuItem>())
					continue;
				CMenuItem* item = (CMenuItem*)subWidget;
				if (item->IsRadio() && item->radioGroupName == radioGroupName)
				{
					item->isToggled = false;
				}
			}
		}

		isToggled = value;
	}

	void CMenuItem::OnBeforeComputeStyle()
	{
		Super::OnBeforeComputeStyle();

		toggleOrRadioStyleState = {};

		auto style = stylesheet->SelectStyle(this, CStateFlag::Default, CSubControl::Check);
		LoadGuiStyleState(style, toggleOrRadioStyleState);
	}

	void CMenuItem::OnClicked()
	{

	}

	void CMenuItem::OnDrawGUI()
    {
		auto rect = GetComputedLayoutRect();
		auto padding = GetComputedLayoutPadding();

		f32 centerY = (rect.min.y + rect.max.y) / 2;

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

		f32 offsetX = 0;
		if (!IsInsideMenuBar() && HasIcon())
			offsetX += 18; // icon offset
		if (siblingExistsWithRadioOrToggle)
			offsetX += 22; // radio/toggle offset

		f32 offsetY = 0;
		if (IsInsideMenuBar())
			offsetY = -5.0f;

		GUI::Text(rect + Rect(padding.left + offsetX, padding.top + offsetY, -padding.right + offsetX, -padding.bottom), text, defaultStyleState);

		if (HasIcon())
		{
			constexpr f32 iconSize = 14;
			Rect iconRect = Rect(rect.left + padding.left, centerY - iconSize / 2,
				rect.left + padding.left + iconSize, centerY + iconSize / 2);
			GUI::Image(iconRect, icon.id, {});
		}

		if (HasSubMenu() && !IsInsideMenuBar() && rightArrowIcon.IsValid())
		{
			const f32 arrowSize = GUI::GetFontSize() * 0.7f;
			f32 arrowPosX = rect.max.x - padding.right - arrowSize;
			f32 arrowPosY = (rect.min.y + rect.max.y) / 2 - arrowSize / 2;
			Rect toggleRect = Rect(arrowPosX, arrowPosY, arrowPosX + arrowSize, arrowPosY + arrowSize);

			GUI::Image(toggleRect, rightArrowIcon.id, {});
		}
		
		if (IsToggle())
		{
			const f32 toggleSize = GUI::GetFontSize() * 0.7f;
			f32 togglePosX = rect.min.x + padding.left;
			f32 togglePosY = (rect.min.y + rect.max.y) / 2 - toggleSize / 2;
			Rect toggleRect = GUI::WindowSpaceToScreenSpace(Rect(togglePosX, togglePosY, togglePosX + toggleSize, togglePosY + toggleSize));
			Rect bgOffset = Rect(-2, -2, 2, 2);

			GUI::FillRect(toggleRect + bgOffset, toggleOrRadioStyleState.background, toggleOrRadioStyleState.borderRadius);
			if (toggleOrRadioStyleState.borderColor.a > 0 && toggleOrRadioStyleState.borderThickness > 0.1f)
				GUI::DrawRect(toggleRect + bgOffset, toggleOrRadioStyleState.borderColor, toggleOrRadioStyleState.borderRadius, toggleOrRadioStyleState.borderThickness);

			if (isToggled)
				GUI::FillCheckMark(Vec2(toggleRect.x, toggleRect.y), defaultStyleState.foreground, toggleSize);
		}
		else if (IsRadio())
		{
			const f32 radioSize = GUI::GetFontSize() * 0.5f;
			f32 togglePosX = rect.min.x + padding.left;
			f32 togglePosY = (rect.min.y + rect.max.y) / 2 - radioSize / 2;
			Rect toggleRect = GUI::WindowSpaceToScreenSpace(Rect(togglePosX, togglePosY, togglePosX + radioSize, togglePosY + radioSize));

			GUI::FillCircle(toggleRect, toggleOrRadioStyleState.background);

			if (isToggled)
				GUI::FillCircle(toggleRect, toggleOrRadioStyleState.foreground);
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
				if (IsToggle() || IsRadio())
				{
					if (IsRadio())
					{
						SetRadioValue(true);
						((CMenu*)parent)->HideAllInChain();
					}
					else
					{
						SetToggleValue(!IsToggled());
					}

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

