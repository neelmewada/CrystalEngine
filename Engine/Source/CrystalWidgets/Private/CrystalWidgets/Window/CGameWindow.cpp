#include "CrystalWidgets.h"

namespace CE::Widgets
{

    CGameWindow::CGameWindow()
    {
        clearScreen = false;
    }

    CGameWindow::~CGameWindow()
    {
	    
    }

    void CGameWindow::Construct()
    {
        Super::Construct();


    }

    void CGameWindow::OnPaint(CPaintEvent* paintEvent)
    {
	    // Do not paint anything
        CPainter* painter = paintEvent->painter;
    }

    void CGameWindow::OnPaintOverlay(CPaintEvent* paintEvent)
    {
        // Do not paint anything
        CPainter* painter = paintEvent->painter;
    }

} // namespace CE::Widgets
