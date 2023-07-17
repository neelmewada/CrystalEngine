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

		assetBrowserPanel = CreateWidget<AssetBrowserPanel>(this, "AssetBrowserPanel");
	}

} // namespace CE::Editor
