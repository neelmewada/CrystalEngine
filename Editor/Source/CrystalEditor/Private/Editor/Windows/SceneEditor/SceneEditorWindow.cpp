#include "CrystalEditor.h"

namespace CE::Editor
{
    SceneEditorWindow::SceneEditorWindow()
    {
		defaultDockPosition = CDockPosition::Fill;
    }

    SceneEditorWindow::~SceneEditorWindow()
    {

    }

    void SceneEditorWindow::Construct()
    {
		Super::Construct();

		SetAsDockSpaceWindow(true);
		SetTitle("Scene Editor");
		SetFullscreen(false);

		assetBrowserPanel = CreateWidget<AssetBrowserPanel>(this, "SceneEditorWindow_AssetBrowserPanel");
		viewportPanel = CreateWidget<RenderViewportPanel>(this, "SceneEditorWindow_ViewportPanel");
		sceneOutlinerPanel = CreateWidget<SceneOutlinerPanel>(this, "SceneEditorWindow_SceneOutlinerPanel");
		detailsPanel = CreateWidget<DetailsPanel>(this, "SceneEditorWindow_DetailsPanel");

		assetBrowserPanel->SetDefaultDockPosition(CDockPosition::Bottom);
		viewportPanel->SetDefaultDockPosition(CDockPosition::Center);
		sceneOutlinerPanel->SetDefaultDockPosition(CDockPosition::Right);
		detailsPanel->SetDefaultDockPosition(CDockPosition::RightBottom);
    }

} // namespace CE::Editor
