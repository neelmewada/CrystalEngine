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

        viewport = CreateObject<CViewport>(this, "ContentViewport");

    }
    
} // namespace CE::Editor
