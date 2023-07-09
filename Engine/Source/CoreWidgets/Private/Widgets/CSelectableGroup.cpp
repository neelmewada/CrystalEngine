#include "CoreWidgets.h"

namespace CE::Widgets
{
	CSelectableGroup::CSelectableGroup()
	{

	}

	CSelectableGroup::~CSelectableGroup()
	{

	}

	bool CSelectableGroup::IsSubWidgetAllowed(ClassType* widgetClass)
	{
		return widgetClass != nullptr && widgetClass->IsSubclassOf<CSelectableWidget>();
	}

	u32 CSelectableGroup::GetSelectedItemIndex()
	{
		for (int i = 0; i < attachedWidgets.GetSize(); i++)
		{
			if (((CSelectableWidget*)attachedWidgets[i])->IsSelected())
				return i;
		}
		return 0;
	}

	bool CSelectableGroup::IsItemSelectedAt(u32 index)
	{
		if (index < attachedWidgets.GetSize() && ((CSelectableWidget*)attachedWidgets[index])->IsSelected())
			return true;
		return false;
	}

	bool CSelectableGroup::SelectItemAt(u32 index)
	{
		if (index < attachedWidgets.GetSize())
		{
			((CSelectableWidget*)attachedWidgets[index])->isSelected = true;
			return true;
		}
		return false;
	}

	void CSelectableGroup::OnDrawGUI()
	{
		auto rect = GetComputedLayoutRect();

		DrawDefaultBackground();

		GUI::WindowFlags flags = GUI::WF_NoMove | GUI::WF_NoBackground;
		if (AlwaysShowVerticalScroll())
			flags |= GUI::WF_AlwaysVerticalScrollbar;

		bool result = GUI::BeginChild(rect, GetUuid(), "", {}, {}, flags);

		if (result)
		{
			GUI::PushZeroingChildCoordinateSpace();

			for (auto child : attachedWidgets)
			{
				child->RenderGUI();
			}

			GUI::PopChildCoordinateSpace();
		}

		GUI::EndChild();
		PollEvents();

	}

	void CSelectableGroup::OnWidgetSelected(CSelectableWidget* selectedWidget)
	{
		for (int i = 0; i < attachedWidgets.GetSize(); i++)
		{
			((CSelectableWidget*)attachedWidgets[i])->isSelected = attachedWidgets[i] == selectedWidget;
		}
	}

} // namespace CE::Widgets
