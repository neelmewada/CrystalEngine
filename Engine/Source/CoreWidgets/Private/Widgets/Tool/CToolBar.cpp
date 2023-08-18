#include "CoreWidgets.h"

namespace CE::Widgets
{

    CToolBar::CToolBar()
    {

    }

    CToolBar::~CToolBar()
    {

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

