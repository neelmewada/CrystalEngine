#include "CoreWidgets.h"

namespace CE::Widgets
{

    CMenuItem::CMenuItem()
    {
		isInteractable = true;
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

		
	}

	bool CMenuItem::HasSubMenu()
	{
		return subMenu != nullptr;
	}

	void CMenuItem::OnDrawGUI()
    {
		auto rect = GetComputedLayoutRect();
		auto padding = GetComputedLayoutPadding();

		DrawDefaultBackground();

		GUI::InvisibleButton(rect, GetUuid());
		PollEvents();

		GUI::Text(rect + Rect(padding.left, padding.top, -padding.right, -padding.bottom), text, defaultStyleState);
    }

	void CMenuItem::HandleEvent(CEvent* event)
	{
		if (!event->ShouldPropagate())
			return;

		if (event->type == CEventType::MouseButtonClick)
		{
			event->HandleAndStopPropagation();
		}
		else if (event->type == CEventType::MouseEnter)
		{
			if (HasSubMenu() && !subMenu->IsShown())
			{
				subMenu->Show(this);
			}

			event->HandleAndStopPropagation();
		}
		else if (event->type == CEventType::MouseLeave)
		{

			event->HandleAndStopPropagation();
		}

		Super::HandleEvent(event);
	}
    
} // namespace CE

