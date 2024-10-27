#include "CrystalEditor.h"

namespace CE::Editor
{

    CrystalEditorWindow::CrystalEditorWindow()
    {

    }

    void CrystalEditorWindow::Construct()
    {
        Super::Construct();

        SceneEditor* sceneEditor = nullptr;
        FAssignNew(SceneEditor, sceneEditor);
        AddDockTab(sceneEditor);

        EditorDockTab* projectSettingsTab = nullptr;
        FAssignNew(EditorDockTab, projectSettingsTab);
        projectSettingsTab->Title("Project Settings");
        AddDockTab(projectSettingsTab);

        Style("EditorDockspace");
    }
    
}

