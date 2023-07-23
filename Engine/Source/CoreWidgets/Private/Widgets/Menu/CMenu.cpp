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

	bool CMenu::RequiresIndependentLayoutCalculation()
	{
		return true;
	}

	void CMenu::OnAfterComputeStyle()
	{
		Super::OnAfterComputeStyle();

	}

	void CMenu::ShowContextMenu()
	{
		Show(GUI::GetMousePos());
	}

	void CMenu::Hide()
	{
		isShown = false;
	}

	void CMenu::Show(Vec2 pos)
	{
		isShown = true;
		pushShow = true;
		this->showPos = pos;

		SetNeedsStyle();
		SetNeedsLayout();
	}

    void CMenu::OnDrawGUI()
    {
		auto size = GetComputedLayoutSize();

		auto viewportSize = GUI::GetMainViewport()->size;

		if (pushShow)
		{
			pushShow = false;
			GUI::OpenPopup(GetUuid());
		}

		if (showPos.x == 0 || showPos.y == 0)
		{
			showPos = GUI::GetMousePos();
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

		GUI::SetNextWindowPos(showPos);
		GUI::SetNextWindowSize(size);

		GUI::WindowFlags flags = GUI::WF_NoMove | GUI::WF_NoResize;
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
		}
		else
		{
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
    
} // namespace CE

