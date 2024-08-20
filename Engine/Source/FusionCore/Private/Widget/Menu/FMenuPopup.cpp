#include "FusionCore.h"

namespace CE
{

    FMenuPopup::FMenuPopup()
    {
        m_AutoClose = true;
        m_BlockInteraction = false;
    }

    bool FMenuPopup::FocusParentExistsRecursive(FWidget* parent)
    {
	    return Super::FocusParentExistsRecursive(parent) || (ownerItem != nullptr && ownerItem->subMenu == this);
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

