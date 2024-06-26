#include "FusionCore.h"

namespace CE
{

    FPopup::FPopup()
    {

    }

    void FPopup::ClosePopup()
    {
        FFusionContext* context = GetContext();
        if (context)
        {
            context->ClosePopup(this);
        }
    }

    void FPopup::HandleEvent(FEvent* event)
    {
        if (event->type == FEventType::FocusChanged)
        {
            FFocusEvent* focusEvent = static_cast<FFocusEvent*>(event);

            if (focusEvent->GotFocus() != IsFocused())
            {
                
            }
        }

	    Super::HandleEvent(event);
    }
}

