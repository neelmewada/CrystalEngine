#include "CoreWidgets.h"

namespace CE::Widgets
{

	CSelectableGroup::CSelectableGroup()
	{

	}

	CSelectableGroup::~CSelectableGroup()
	{

	}

	void CSelectableGroup::OnSubWidgetAttached(CWidget* widget)
	{
		if (!widget->GetClass()->IsSubclassOf<CSelectableWidget>())
		{
			throw CWidgetException("Attempted to add an illegal subwidget to CSelectableGroup");
		}

		Super::OnSubWidgetAttached(widget);
	}

	void CSelectableGroup::OnDrawGUI()
	{
		Super::OnDrawGUI();

		for (int i = 0; i < attachedWidgets.GetSize(); i++)
		{
			CSelectableWidget* widget = (CSelectableWidget*)attachedWidgets[i];
			if (widget->IsSelected() && selectedIndex != i)
			{
				selectedIndex = i;
				break;
			}
		}

		for (int i = 0; i < attachedWidgets.GetSize(); i++)
		{
			CSelectableWidget* widget = (CSelectableWidget*)attachedWidgets[i];
			widget->Select(selectedIndex == i);
		}
	}

	void CSelectableGroup::HandleEvent(CEvent* event)
	{

		Super::HandleEvent(event);
	}

} // namespace CE::Widgets

