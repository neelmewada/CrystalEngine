#include "CoreWidgets.h"

namespace CE::Widgets
{

    CMenuBar::CMenuBar()
    {

    }

    CMenuBar::~CMenuBar()
    {
        
    }

	bool CMenuBar::OnSubMenuItemHovered(CMenuItem* subMenuItem)
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

	bool CMenuBar::IsAnySubMenuOpen()
	{
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

	void CMenuBar::OnDrawGUI()
    {
		auto rect = GetComputedLayoutRect();

		DrawDefaultBackground();

		GUI::WindowFlags flags = GUI::WF_NoMove | GUI::WF_NoBackground | GUI::WF_NoResize;
		flags |= GUI::WF_HorizontalScrollbar;
		flags |= GUI::WF_NoScrollWithMouse;
		flags |= GUI::WF_NoScrollbar;

		//bool result = GUI::BeginChild(rect, GetUuid(), "", {}, {}, flags);
		bool result = GUI::BeginMenuBar();

		if (result)
		{
			auto parent = GetOwner();
			Rect offset = {};
			if (parent != nullptr && parent->IsOfType<CWindow>())
			{
				CWindow* window = (CWindow*)parent;
				if (window->GetMenuBar() != nullptr)
				{
					offset = Rect(0, 1, 0, 1) * window->GetTabPadding().y * GUI::GetFontSize() * 0.25f;
				}
			}

			GUI::PushChildCoordinateSpace(rect + offset);

			for (auto child : attachedWidgets)
			{
				child->Render();
			}

			GUI::PopChildCoordinateSpace();
		}

		//GUI::EndChild();
		GUI::EndMenuBar();
		PollEvents();
    }

	void CMenuBar::HandleEvent(CEvent* event)
	{
		if (event->type == CEventType::MouseButtonClick)
		{
			if (event->sender != nullptr && event->sender->GetClass()->IsSubclassOf<CMenuItem>())
			{
				
			}
		}
		else if (event->type == CEventType::MouseEnter)
		{
			if (event->sender != nullptr && event->sender->GetClass()->IsSubclassOf<CMenuItem>())
			{
				CMenuItem* menuItem = (CMenuItem*)event->sender;

				if (IsAnySubMenuOpen() && OnSubMenuItemHovered(menuItem))
					event->HandleAndStopPropagation();
			}
		}
		else if (event->type == CEventType::MouseLeave)
		{

		}

		Super::HandleEvent(event);
	}
    
} // namespace CE

