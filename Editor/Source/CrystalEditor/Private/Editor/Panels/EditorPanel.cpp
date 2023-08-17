
#include "CrystalEditor.h"

static const CE::String css = R"(
EditorPanel {
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

