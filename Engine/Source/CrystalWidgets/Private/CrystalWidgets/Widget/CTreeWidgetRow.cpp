#include "CrystalWidgets.h"

namespace CE::Widgets
{

	CTreeWidgetRow::CTreeWidgetRow()
	{
		
	}

	CTreeWidgetRow::~CTreeWidgetRow()
	{
		
	}

	void CTreeWidgetRow::SetAlternate(bool set)
	{
		CSubControl subControl = set ? CSubControl::Alternate : CSubControl::None;
		if (this->subControl == subControl)
			return;
		
		this->subControl = subControl;

		SetNeedsStyle();
		SetNeedsPaint();
	}

} // namespace CE::Widgets
