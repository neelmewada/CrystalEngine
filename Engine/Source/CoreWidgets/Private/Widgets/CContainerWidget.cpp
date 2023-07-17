#include "CoreWidgets.h"

namespace CE::Widgets
{
    CContainerWidget::CContainerWidget()
    {

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

} // namespace CE::Widgets
