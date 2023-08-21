#include "CoreWidgets.h"

namespace CE::Widgets
{

    CToolBar::CToolBar()
    {

    }

    CToolBar::~CToolBar()
    {

    }

	Vec2 CToolBar::CalculateIntrinsicContentSize(f32 width, f32 height)
	{
		Vec2 size = Vec2(10, 15);
		if (!IsNan(width) && width > 0)
		{
			size.x = width;
		}
		return size;
	}

	void CToolBar::OnDrawGUI()
    {
		auto rect = GetComputedLayoutRect();

		GUI::WindowFlags flags = GUI::WF_NoTitleBar | GUI::WF_NoMove | GUI::WF_NoDocking | GUI::WF_NoPadding | GUI::WF_NoScrollbar;

		GUI::SetCursorPos(GUI::WidgetSpaceToWindowSpace(rect.min));

		if (GUI::BeginChild(rect, GetUuid(), "ToolBar", {}, defaultStyleState, flags))
		{
			//GUI::PushChildCoordinateSpace(rect);
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

