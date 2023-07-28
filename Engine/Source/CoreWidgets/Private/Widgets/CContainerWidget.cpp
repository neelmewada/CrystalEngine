#include "CoreWidgets.h"

namespace CE::Widgets
{
    CContainerWidget::CContainerWidget()
    {
		isInteractable = false;
    }

    CContainerWidget::~CContainerWidget()
    {

    }

    void CContainerWidget::OnDrawGUI()
    {
		auto rect = GetComputedLayoutRect();

		DrawDefaultBackground();

		GUI::WindowFlags flags = GUI::WF_NoMove | GUI::WF_NoBackground | GUI::WF_NoResize;
		if (allowHorizontalScroll)
			flags |= GUI::WF_HorizontalScrollbar;
		if (!allowVerticalScroll)
			flags |= GUI::WF_NoScrollWithMouse;
		if (!allowVerticalScroll && !allowHorizontalScroll)
			flags |= GUI::WF_NoScrollbar;

		bool result = GUI::BeginChild(rect, GetUuid(), "", {}, {}, flags);

		if (result)
		{
			GUI::PushZeroingChildCoordinateSpace();

			for (int i = 0; i < attachedWidgets.GetSize(); i++)
			{
				attachedWidgets[i]->Render();
			}

			GUI::PopChildCoordinateSpace();
		}

		GUI::EndChild();
		PollEvents();
    }

    void CContainerWidget::HandleEvent(CEvent* event)
    {
		Super::HandleEvent(event);
    }

	bool CContainerWidget::TestFocus()
	{
		return GUI::IsWindowFocused(GetUuid(), GUI::FOCUS_ChildWindows);
	}

} // namespace CE::Widgets
