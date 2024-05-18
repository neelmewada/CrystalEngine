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

        CPen pen = CPen(Color::Red());
        pen.SetWidth(1.0f);

        painter->SetPen(pen);

        painter->SetRotation(30);

        painter->DrawRect(Rect::FromSize(0, 0, 100, 100));
        
        painter->DrawLine(Vec2(10, 0), Vec2(100, 100));

        painter->PopChildCoordinateSpace();
    }

} // namespace CE::Editor
