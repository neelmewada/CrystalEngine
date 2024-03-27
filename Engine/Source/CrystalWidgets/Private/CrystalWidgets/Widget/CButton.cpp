#include "CrystalWidgets.h"

namespace CE::Widgets
{

    CButton::CButton()
    {
        receiveDragEvents = false;
        receiveMouseEvents = true;
        allowVerticalScroll = allowHorizontalScroll = false;
    }

    CButton::~CButton()
    {
        CE::GetTypeId<String>();
    }

    void CButton::HandleEvent(CEvent* event)
    {
        if (event->IsMouseEvent() && !event->isConsumed)
        {
            CMouseEvent* mouseEvent = (CMouseEvent*)event;
            bool wasPressedInside = EnumHasFlag(stateFlags, CStateFlag::Pressed);

            if (event->type == CEventType::MouseRelease && wasPressedInside)
            {
                event->Consume(this);
                OnButtonClicked(this, mouseEvent->button);
            }
        }

        Super::HandleEvent(event);
    }
    
} // namespace CE::Widgets
