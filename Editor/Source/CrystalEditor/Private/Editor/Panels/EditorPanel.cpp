
#include "CrystalEditor.h"

const CE::String css = R"(
EditorPanel {
	background: black;
	align-items: stretch;
	align-content: stretch;
}

EditorPanelView {
	background: red;
	align-items: stretch;
	align-content: stretch;
}
)";

namespace CE::Editor
{

	EditorPanel::EditorPanel()
	{
		
	}

	EditorPanel::~EditorPanel()
	{

	}

	void EditorPanel::Construct()
	{
		Super::Construct();

		SetStyleSheet(css);
	}

	bool EditorPanel::IsSubWidgetAllowed(ClassType* subwidgetClass)
	{
		return subwidgetClass != nullptr && subwidgetClass->IsSubclassOf<EditorPanelView>();
	}

	void EditorPanel::OnDrawGUI()
	{
		Super::OnDrawGUI();
	}
}

