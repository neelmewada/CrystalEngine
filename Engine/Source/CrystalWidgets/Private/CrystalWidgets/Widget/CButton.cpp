#include "CrystalWidgets.h"

namespace CE::Widgets
{

    CButton::CButton()
    {
        receiveDragEvents = false;
        receiveMouseEvents = true;
        allowVerticalScroll = allowHorizontalScroll = false;
        clipChildren = true;

        label = CreateDefaultSubobject<CLabel>("Label");
    }

    CButton::~CButton()
    {
        
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
                OnButtonLeftClicked();
                OnButtonClicked(mouseEvent->button);
            }
        }

        Super::HandleEvent(event);
    }
    
} // namespace CE::Widgets
