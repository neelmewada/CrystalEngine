#include "CrystalEditor.h"

static const CE::String css = R"(
EditorWindow::tab {
	padding: 10px 7px;
}
)";

namespace CE::Editor
{
    EditorWindow::EditorWindow()
    {
		tabPadding = Vec2(5, 5);
    }

    EditorWindow::~EditorWindow()
    {

    }

	void EditorWindow::Construct()
	{
		Super::Construct();

		menuBar = CreateWidget<CMenuBar>(this, "EditorMenuBar");
		//toolBar = CreateWidget<CToolBar>(this, "EditorToolBar");

		SetStyleSheet(css);
	}

	void EditorWindow::OnDrawGUI()
    {
		Super::OnDrawGUI();

    }

} // namespace CE::Editor
