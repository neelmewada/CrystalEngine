#include "CoreWidgets.h"

namespace CE::Widgets
{
	CLayoutGroup::CLayoutGroup()
	{

	}

	CLayoutGroup::~CLayoutGroup()
	{

	}

	void CLayoutGroup::OnDrawGUI()
	{
		auto rect = GetComputedLayoutRect();

		DrawDefaultBackground();

		GUI::PushChildCoordinateSpace(rect);

		for (auto child : attachedWidgets)
		{
			child->Render();
		}

		GUI::PopChildCoordinateSpace();
	}

} // namespace CE::Widgets
