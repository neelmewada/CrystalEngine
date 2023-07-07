#include "CoreWidgets.h"

namespace CE::Widgets
{
	CSelectableWidget::CSelectableWidget()
	{

	}

	CSelectableWidget::~CSelectableWidget()
	{

	}

	void CSelectableWidget::OnDrawGUI()
	{
		auto rect = GetComputedLayoutRect();
		auto padding = GetComputedLayoutPadding();

		DrawDefaultBackground();

		GUI::PushChildCoordinateSpace(rect);

		for (auto child : attachedWidgets)
		{
			child->RenderGUI();
		}

		GUI::PopChildCoordinateSpace();
	}

} // namespace CE::Widgets
