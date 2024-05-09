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

        sceneEditor = CreateObject<SceneEditorWindow>(root, "SceneEditor");
        viewportWindow = sceneEditor->GetViewportWindow();

        sceneSubsystem = gEngine->GetSubsystem<SceneSubsystem>();
        sceneSubsystem->SetMainViewport(viewportWindow->GetViewport());

    }

    void CrystalEditorWindow::OnBeforeDestroy()
    {
	    Super::OnBeforeDestroy();

        if (sceneSubsystem && sceneSubsystem->GetMainViewport() == viewportWindow->GetViewport())
        {
            sceneSubsystem->SetMainViewport(nullptr);
        }
    }

} // namespace CE::Editor
