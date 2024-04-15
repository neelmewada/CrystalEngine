#include "CrystalEditor.h"

namespace CE::Editor
{

    CrystalEditorWindow::CrystalEditorWindow()
    {
        dockType = CDockType::Major;
    }

    CrystalEditorWindow::~CrystalEditorWindow()
    {
	    
    }

    void CrystalEditorWindow::Construct()
    {
	    Super::Construct();

        SetTitle("Crystal Editor");

        CDockSplitView* root = GetRootDockSplit();

        SceneEditorWindow* sceneEditor = CreateObject<SceneEditorWindow>(root, "SceneEditor");
        
    }

} // namespace CE::Editor
