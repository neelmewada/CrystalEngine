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

		GUI::WindowFlags flags = GUI::WF_NoBackground | GUI::WF_NoTitleBar | GUI::WF_NoMove | GUI::WF_NoDocking | GUI::WF_NoPadding;

		//GUI::BeginGroup();
		GUI::PushStyleVar(GUI::StyleVar_FramePadding, Vec2());
		GUI::BeginChild(rect, GetUuid(), "ToolBar", {}, defaultStyleState, flags);

		GUI::PushZeroingChildCoordinateSpace();
		for (auto subwidget : attachedWidgets)
		{
			subwidget->Render();
		}
		GUI::PopChildCoordinateSpace();

		GUI::EndChild();
		GUI::PopStyleVar();
		//GUI::EndGroup();
		PollEvents();
    }

} // namespace CE::Widgets

