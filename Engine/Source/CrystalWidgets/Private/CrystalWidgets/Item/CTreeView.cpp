#include "CrystalWidgets.h"

namespace CE::Widgets
{

	CTreeView::CTreeView()
	{
		canDrawRowHeader = false;
	}

	CTreeView::~CTreeView()
	{
		
	}

	void CTreeView::OnPaint(CPaintEvent* paintEvent)
	{
		Super::OnPaint(paintEvent);


	}

	void CTreeView::OnPaintOverlay(CPaintEvent* paintEvent)
	{
		Super::OnPaintOverlay(paintEvent);

	}

} // namespace CE::Widgets
