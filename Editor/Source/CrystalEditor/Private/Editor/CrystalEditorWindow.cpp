#include "CrystalEditor.h"

namespace CE::Editor
{
	CrystalEditorWindow::CrystalEditorWindow()
	{
		
	}

	CrystalEditorWindow::~CrystalEditorWindow()
	{

	}

	void CrystalEditorWindow::Construct()
	{
		SetAsDockSpaceWindow(true);
		SetTitle("Crystal Editor");
		SetFullscreen(true);

		Super::Construct();

		auto assetBrowserPanel = CreateWidget<EditorPanel>(this, "AssetBrowserPanel");
		assetBrowserPanel->SetTitle("Asset Browser");

		assetBrowserView = CreateWidget<AssetBrowserView>(assetBrowserPanel, "AssetBrowserView");
		
	}

} // namespace CE::Editor
