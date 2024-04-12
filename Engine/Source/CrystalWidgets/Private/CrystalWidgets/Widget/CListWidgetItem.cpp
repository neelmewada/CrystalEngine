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

            if (event->type == CEventType::MousePress && owner && mouseEvent->button == MouseButton::Left)
            {
                owner->OnItemClicked(this, mouseEvent->keyModifiers);
            }
        }

	    Super::HandleEvent(event);
    }

} // namespace CE::Widgets
