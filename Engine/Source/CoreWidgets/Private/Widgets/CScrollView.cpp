#include "CoreWidgets.h"

namespace CE::Widgets
{
	CScrollView::CScrollView()
	{

	}

	CScrollView::~CScrollView()
	{

	}

	void CScrollView::OnDrawGUI()
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
				child->Render();
			}

			GUI::PopChildCoordinateSpace();
		}

		GUI::EndChild();
		PollEvents();
	}

} // namespace CE::Widgets
