#include "CrystalWidgets.h"

namespace CE::Widgets
{

    CListWidgetItem::CListWidgetItem()
    {
        receiveMouseEvents = true;
    }

    CListWidgetItem::~CListWidgetItem()
    {
	    
    }

    void CListWidgetItem::HandleEvent(CEvent* event)
    {
        if (event->IsMouseEvent())
        {
            CMouseEvent* mouseEvent = static_cast<CMouseEvent*>(event);

            if (event->type == CEventType::MousePress && owner)
            {
                owner->Select(this);
            }
        }

	    Super::HandleEvent(event);
    }

} // namespace CE::Widgets
