#include "FusionCore.h"

namespace CE
{

    FMenuPopup::FMenuPopup()
    {
        m_AutoClose = true;
        m_BlockInteraction = false;
    }

    void FMenuPopup::Construct()
    {
	    Super::Construct();

        Child(
            FAssignNew(FVerticalStack, container)
        );
    }

}

