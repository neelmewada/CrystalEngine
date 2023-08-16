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

		assetBrowserPanel = CreateWidget<AssetBrowserPanel>(this, "AssetBrowserPanel");
		renderViewport = CreateWidget<RenderViewportPanel>(this, "RenderViewportPanel");
    }

} // namespace CE::Editor
