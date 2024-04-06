#include "CrystalWidgets.h"

namespace CE::Widgets
{

	CTreeView::CTreeView()
	{
		canDrawRowHeader = false;
		selectionType = CItemSelectionType::SelectRow;
		selectionMode = CItemSelectionMode::ExtendedSelection;
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
