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

        painter->PushChildCoordinateSpace(GetComputedLayoutTopLeft());

        painter->SetBrush(CBrush(Color::White()));

        CPen pen = CPen(Color::White());
        pen.SetWidth(1.0f);
        pen.SetStyle(CPenStyle::DashedLine);
        pen.SetDashLength(5.0f);
        painter->SetPen(pen);
        
        painter->DrawLine(Vec2(100, 100), Vec2(100, 250));

        painter->PopChildCoordinateSpace();
    }

} // namespace CE::Editor
