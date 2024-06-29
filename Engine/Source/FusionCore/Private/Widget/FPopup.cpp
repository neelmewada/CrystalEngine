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
                if (focusEvent->GotFocus())
                {
					//CE_LOG(Info, All, "Focus Got!");
                }
                else
                {
                    if (AutoClose())
                    {
                        ClosePopup();
                    }

                    String name = "NULL";
                    if (focusEvent->focusedWidget != nullptr)
                    {
                        name = focusEvent->focusedWidget->GetName().GetString();
                    }
                    
					//CE_LOG(Info, All, "Focus Lost to {}", name);
                }
            }
        }

	    Super::HandleEvent(event);
    }
}

