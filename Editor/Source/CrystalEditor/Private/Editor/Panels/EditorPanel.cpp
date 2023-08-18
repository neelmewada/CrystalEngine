
#include "CrystalEditor.h"

static const CE::String css = R"(
EditorPanel::tab {
	padding: 5px 5px;
}
EditorPanel {
	padding: 0 30px 0 0;
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

	void EditorPanel::OnDrawGUI()
	{
		Super::OnDrawGUI();
	}
}

