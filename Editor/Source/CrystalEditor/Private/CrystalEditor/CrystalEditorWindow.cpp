#include "CrystalEditor.h"

namespace CE::Editor
{

    CrystalEditorWindow::CrystalEditorWindow()
    {

    }

    void CrystalEditorWindow::Construct()
    {
        Super::Construct();

        Style("EditorDockspace");
    }
    
}

