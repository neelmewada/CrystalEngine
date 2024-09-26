#include "FusionCore.h"

namespace CE
{

    FButton::FButton()
    {

    }

    void FButton::SetInteractionEnabled(bool enabled)
    {
        if (IsInteractionDisabled() == !enabled)
            return;

        if (enabled)
        {
            buttonState &= ~FButtonState::InteractionDisabled;
        }
        else
        {
            buttonState = FButtonState::InteractionDisabled;
        }

        ApplyStyle();
    }

    void FButton::HandleEvent(FEvent* event)
    {
        if (event->IsMouseEvent() && !IsInteractionDisabled())
        {
            FMouseEvent* mouseEvent = static_cast<FMouseEvent*>(event);

            if (mouseEvent->type == FEventType::MouseEnter && event->sender == this)
            {
	            if (!EnumHasFlag(buttonState, FButtonState::Hovered))
	            {
                    buttonState |= FButtonState::Hovered;
                    ApplyStyle();
	            }
                event->Consume(this);
            }
            else if (mouseEvent->type == FEventType::MouseLeave && event->sender == this)
            {
	            if (EnumHasFlag(buttonState, FButtonState::Hovered))
	            {
                    buttonState &= ~FButtonState::Hovered;
                    ApplyStyle();
	            }
                event->Consume(this);
            }
            else if (mouseEvent->type == FEventType::MousePress && mouseEvent->buttons == MouseButtonMask::Left)
            {
	            if (!EnumHasFlag(buttonState, FButtonState::Pressed))
	            {
                    buttonState |= FButtonState::Pressed;
                    ApplyStyle();
	            }
                event->Consume(this);
            }
            else if (mouseEvent->type == FEventType::MouseRelease && mouseEvent->buttons == MouseButtonMask::Left)
            {
                if (EnumHasFlag(buttonState, FButtonState::Pressed))
                {
                    buttonState &= ~FButtonState::Pressed;
                    ApplyStyle();

                    if (mouseEvent->isInside)
                    {
                        m_OnPressed();
                    }
                }
                event->Consume(this);
            }
        }

	    Super::HandleEvent(event);
    }

    void FButton::OnPaintContentOverlay(FPainter* painter)
    {
	    Super::OnPaintContentOverlay(painter);

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

