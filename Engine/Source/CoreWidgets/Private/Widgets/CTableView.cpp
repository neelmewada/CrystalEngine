#include "CoreWidgets.h"

namespace CE::Widgets
{

    CTableView::CTableView()
    {
        
    }

    CTableView::~CTableView()
    {
        
    }

	Vec2 CTableView::CalculateIntrinsicContentSize(f32 width, f32 height)
	{
		return Vec2(width, height);
	}

    void CTableView::OnDrawGUI()
    {

    }

    void CTableView::HandleEvent(CEvent* event)
    {

		Super::HandleEvent(event);
    }
    
} // namespace CE::Widgets
