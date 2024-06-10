#include "FusionCore.h"

namespace CE
{

    FButton::FButton()
    {

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

