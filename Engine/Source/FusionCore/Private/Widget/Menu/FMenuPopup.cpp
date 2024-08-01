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

    void FMenuPopup::OnPopupClosed()
    {
	    Super::OnPopupClosed();

        for (FMenuItem* menuItem : menuItems)
        {
	        if (menuItem->subMenu)
	        {
                menuItem->subMenu->ClosePopup();
	        }
        }
    }
}

