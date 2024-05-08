#include "CrystalWidgets.h"

namespace CE::Widgets
{

    CViewport::CViewport()
    {
	    
    }

    CViewport::~CViewport()
    {
	    
    }

    void CViewport::OnPaint(CPaintEvent* paintEvent)
    {
        CPainter* painter = paintEvent->painter;

        

	    Super::OnPaint(paintEvent);
    }

} // namespace CE::Widgets
