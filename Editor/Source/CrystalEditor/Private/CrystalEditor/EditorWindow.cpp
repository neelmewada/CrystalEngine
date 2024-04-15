#include "CrystalEditor.h"

namespace CE::Editor
{

    EditorWindow::EditorWindow()
    {
	    
    }

    EditorWindow::~EditorWindow()
    {
	    
    }

    void EditorWindow::Construct()
    {
        Super::Construct();

        toolBar = CreateObject<EditorToolBar>(this, "ToolBar");
        
    }

    
} // namespace CE::Editor
