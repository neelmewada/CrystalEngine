#include "CrystalEditor.h"

namespace CE::Editor
{

    ViewportWindow::ViewportWindow()
    {
	    
    }

    ViewportWindow::~ViewportWindow()
    {
	    
    }

    void ViewportWindow::Construct()
    {
        Super::Construct();

        SetTitle("Viewport");

        viewport = CreateObject<EditorViewport>(this, "ContentViewport");
    }
    
} // namespace CE::Editor
