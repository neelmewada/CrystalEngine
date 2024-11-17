#include "CrystalEditor.h"

namespace CE::Editor
{
    static CrystalEditorWindow* instance = nullptr;

	CrystalEditorWindow* CrystalEditorWindow::Get()
	{
        return instance;
	}

    CrystalEditorWindow::CrystalEditorWindow()
    {

    }

    void CrystalEditorWindow::Construct()
    {
        Super::Construct();

        instance = this;

        SceneEditor* sceneEditor = nullptr;
        FAssignNew(SceneEditor, sceneEditor);
        AddDockTab(sceneEditor);

        ProjectSettings* projectSettings = GetSettings<ProjectSettings>();

        projectLabelParent->Enabled(true);
        projectLabel->Text(projectSettings->projectName);

        Style("EditorDockspace");
    }

    void CrystalEditorWindow::OnBeginDestroy()
    {
	    Super::OnBeginDestroy();

        if (this == instance)
        {
            instance = nullptr;
        }
    }
}

