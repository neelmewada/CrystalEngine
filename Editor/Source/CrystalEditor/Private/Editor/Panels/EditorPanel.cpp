
#include "CrystalEditor.h"

static const CE::String css = R"(
EditorPanel::tab {
	padding: 5px 5px;
}
EditorPanel {
	padding: 0 0 0 0;
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

    Vec2 EditorPanel::CalculateIntrinsicContentSize(f32 width, f32 height)
    {
		return Super::CalculateIntrinsicContentSize(width, height);
    }

    void EditorPanel::Construct()
	{
		Super::Construct();

		SetStyleSheet(css);
	}

	void EditorPanel::UpdateLayoutIfNeeded()
	{
		Super::UpdateLayoutIfNeeded();
	}

	void EditorPanel::OnDrawGUI()
	{
		Super::OnDrawGUI();
	}
}

