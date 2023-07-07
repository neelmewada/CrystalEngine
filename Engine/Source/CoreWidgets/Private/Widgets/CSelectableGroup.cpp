#include "CoreWidgets.h"

namespace CE::Widgets
{
	CSelectableGroup::CSelectableGroup()
	{

	}

	CSelectableGroup::~CSelectableGroup()
	{

	}

	void CSelectableGroup::OnDrawGUI()
	{
		auto rect = GetComputedLayoutRect();

		DrawDefaultBackground();

		GUI::PushChildCoordinateSpace(rect);

		for (auto child : attachedWidgets)
		{
			child->RenderGUI();
		}

		GUI::PopChildCoordinateSpace();
	}

} // namespace CE::Widgets
