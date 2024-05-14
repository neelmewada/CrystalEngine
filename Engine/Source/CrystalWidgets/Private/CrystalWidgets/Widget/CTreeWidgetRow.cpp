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

	void CTreeWidgetRow::HandleEvent(CEvent* event)
	{
		if (event->IsMouseEvent())
		{
			CMouseEvent* mouseEvent = static_cast<CMouseEvent*>(event);

			if (event->type == CEventType::MousePress && ownerItem)
			{
				treeWidget->Select(ownerItem);
			}
		}

		Super::HandleEvent(event);
	}

} // namespace CE::Widgets
