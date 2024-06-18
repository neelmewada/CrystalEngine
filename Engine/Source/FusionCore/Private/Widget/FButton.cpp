#include "FusionCore.h"

namespace CE
{

    FButton::FButton()
    {

    }

    void FButton::HandleEvent(FEvent* event)
    {
        if (event->IsMouseEvent())
        {
            FMouseEvent* mouseEvent = static_cast<FMouseEvent*>(event);

            if (mouseEvent->type == FEventType::MouseEnter)
            {
	            if (!EnumHasFlag(buttonState, FButtonState::Hovered))
	            {
                    buttonState |= FButtonState::Hovered;
                    ApplyStyle();
	            }
            }
            else if (mouseEvent->type == FEventType::MouseLeave)
            {
	            if (EnumHasFlag(buttonState, FButtonState::Hovered))
	            {
                    buttonState &= ~FButtonState::Hovered;
                    ApplyStyle();
	            }
            }
            else if (mouseEvent->type == FEventType::MousePress && mouseEvent->mouseButtons == MouseButtonMask::Left)
            {
	            if (!EnumHasFlag(buttonState, FButtonState::Pressed))
	            {
                    buttonState |= FButtonState::Pressed;
                    ApplyStyle();
	            }
            }
            else if (mouseEvent->type == FEventType::MouseRelease && mouseEvent->mouseButtons == MouseButtonMask::Left)
            {
                if (EnumHasFlag(buttonState, FButtonState::Pressed))
                {
                    buttonState &= ~FButtonState::Pressed;
                    ApplyStyle();
                }
            } // TODO: Check for release outside

            event->Consume(this);
        }

	    Super::HandleEvent(event);
    }

    void FButton::SetState(FButtonState newState)
    {
        if (buttonState == newState)
            return;

        buttonState = newState;

        if (m_Style)
        {
            m_Style->MakeStyle(*this);
        }
    }

}

