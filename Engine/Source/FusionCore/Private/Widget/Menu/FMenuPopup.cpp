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

    void FMenuPopup::HandleEvent(FEvent* event)
    {
        if (event->type == FEventType::FocusChanged)
        {
            FFocusEvent* focusEvent = static_cast<FFocusEvent*>(event);

            if (focusEvent->LostFocus() && AutoClose())
            {
                FWidget* focusedWidget = focusEvent->focusedWidget;

                ClosePopup();

                if (focusedWidget != nullptr)
                {
                    FMenuItem* ownerItem = this->ownerItem;

                    while (ownerItem != nullptr)
                    {
                        FWidget* menuOwner = ownerItem->menuOwner;
                        if (menuOwner == nullptr)
                            break;

                        if (menuOwner->IsOfType<FMenuPopup>())
                        {
                            FMenuPopup* menuOwnerPopup = static_cast<FMenuPopup*>(menuOwner);

                            if (!focusedWidget->ParentExistsRecursive(menuOwnerPopup))
                            {
                                menuOwnerPopup->ClosePopup();
                            }

                            ownerItem = menuOwnerPopup->ownerItem;
                        }
                        else if (menuOwner->IsOfType<FMenuBar>())
                        {
                            ownerItem = nullptr;
                        }
                    }
                }
            }
        }

	    Super::HandleEvent(event);
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

        if (ownerItem != nullptr)
        {
            ownerItem->isHovered = false;
            if (ownerItem->menuOwner)
                ownerItem->menuOwner->ApplyStyle();
        }

        for (FMenuItem* menuItem : menuItems)
        {
	        if (menuItem->subMenu)
	        {
                menuItem->subMenu->ClosePopup();
	        }
        }
    }
}

