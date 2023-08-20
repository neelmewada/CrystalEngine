#include "CrystalEditor.h"

static const CE::String css = R"(
#HorizontalLayout {
	margin: 0 5px 0 0;
	flex-direction: row;
	align-items: center;
}
)";


namespace CE::Editor
{
	static CrystalEditorWindow* gEditorWindow = nullptr;

	CrystalEditorWindow::CrystalEditorWindow()
	{
		tabPadding = Vec2(10, 10);
	}

	CrystalEditorWindow::~CrystalEditorWindow()
	{
		gEditorWindow = nullptr;
	}

	CrystalEditorWindow* CrystalEditorWindow::Get()
	{
		return gEditorWindow;
	}

	void CrystalEditorWindow::Construct()
	{
		gEditorWindow = this;
		
		LoadStyleSheet("/CrystalEditor/Resources/Styles/CrystalEditorWindow.css");

		SetAsDockSpaceWindow(true);
		SetTitle("Crystal Editor");
		SetFullscreen(true);

		Super::Construct();

		sceneEditorWindow = CreateWidget<SceneEditorWindow>(this, "SceneEditor");
	}

	void CrystalEditorWindow::OnDrawGUI()
	{
		Super::OnDrawGUI();

		GetWidgetDebugger()->RenderGUI();
	}

} // namespace CE::Editor
