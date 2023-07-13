
#include "CrystalEditor.h"

namespace CE::Editor
{
	EditorPanelView::EditorPanelView()
	{

	}

	EditorPanelView::~EditorPanelView()
	{

	}

	void EditorPanelView::OnDrawGUI()
	{
		auto rect = GetComputedLayoutRect();

		DrawDefaultBackground();

		GUI::PushChildCoordinateSpace(rect);

		for (auto subwidget : attachedWidgets)
		{
			subwidget->RenderGUI();
		}

		GUI::PopChildCoordinateSpace();
	}
}

