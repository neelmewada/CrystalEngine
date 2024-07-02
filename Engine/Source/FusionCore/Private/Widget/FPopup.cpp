#include "FusionCore.h"

namespace CE
{

    FPopup::FPopup()
    {
        m_BlockInteraction = false;
        m_AutoClose = true;
    }

    void FPopup::ClosePopup()
    {
        if (!isShown)
            return;

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
        else if (event->type == FEventType::NativeWindowExposed && m_AutoClose)
        {
            FNativeEvent* nativeEvent = static_cast<FNativeEvent*>(event);
            FFusionContext* context = GetContext();

            if ((isNativePopup && context != nullptr && context->ParentContextExistsRecursive(nativeEvent->nativeContext))||
                (!isNativePopup && context == nativeEvent->nativeContext))
            {
                ClosePopup();
            }
        }

	    Super::HandleEvent(event);
    }
}

