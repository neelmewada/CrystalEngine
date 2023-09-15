#include "CrystalEditor.h"

static const CE::String css = R"(
EditorWindow::tab {
	padding: 10px 7px;
}
)";

namespace CE::Editor
{
	HashMap<Name, Array<ClassType*>> EditorWindow::assetDefNameToEditorWindowClass{};

    EditorWindow::EditorWindow()
    {
		tabPadding = Vec2(5, 5);
    }

    EditorWindow::~EditorWindow()
    {

    }

	ClassType* EditorWindow::GetEditorWindowClassForAssetDef(const Name& assetDefClassName)
	{
		if (assetDefNameToEditorWindowClass.KeyExists(assetDefClassName))
		{
			auto& array = assetDefNameToEditorWindowClass[assetDefClassName];
			if (array.IsEmpty())
				return nullptr;
			return array.Top();
		}
		return nullptr;
	}

	void EditorWindow::Construct()
	{
		Super::Construct();

		if (ShouldCreateMenuBar())
		{
			menuBar = CreateWidget<CMenuBar>(this, "EditorMenuBar");
		}

		if (ShouldCreateToolBar())
		{
			toolBar = CreateWidget<CToolBar>(this, "EditorToolBar");
		}

		SetStyleSheet(css);
	}

	void EditorWindow::OnDrawGUI()
    {
		Super::OnDrawGUI();
    }

} // namespace CE::Editor
