#include "CrystalEditor.h"

namespace CE::Editor
{

    ViewportWindow::ViewportWindow()
    {
        allowHorizontalScroll = allowVerticalScroll = false;
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
