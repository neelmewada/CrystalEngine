#include "CrystalEditor.h"

namespace CE::Editor
{

    AssetBrowserWindow::AssetBrowserWindow()
    {
	    
    }

    AssetBrowserWindow::~AssetBrowserWindow()
    {
	    
    }

    void AssetBrowserWindow::Construct()
    {
        Super::Construct();

        SetTitle("Assets");
    }

    void AssetBrowserWindow::OnPaint(CPaintEvent* paintEvent)
    {
	    Super::OnPaint(paintEvent);

        CPainter* painter = paintEvent->painter;


    }

} // namespace CE::Editor
