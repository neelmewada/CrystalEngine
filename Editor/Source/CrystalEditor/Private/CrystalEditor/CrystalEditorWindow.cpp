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

        Style("EditorDockspace");
    }
    
}

