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

		GUI::BeginGroup();

		GUI::PushChildCoordinateSpace(rect);
		for (auto subwidget : attachedWidgets)
		{
			subwidget->Render();
		}
		GUI::PopChildCoordinateSpace();

		GUI::EndGroup();
		PollEvents();
    }

} // namespace CE::Widgets

