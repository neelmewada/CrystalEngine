#include "FusionCore.h"

namespace CE
{

    FMenuPopup::FMenuPopup()
    {
        m_AutoClose = true;
        m_BlockInteraction = false;

        m_Padding = Vec4(1, 1, 1, 1);
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

                if (focusedWidget != nullptr)
                {
                    FMenuItem* ownerItem = this->ownerItem;

                    if (ownerItem && !focusedWidget->ParentExistsRecursive(ownerItem))
                    {
                        ClosePopup();
                    }

                    while (ownerItem != nullptr)
                    {
                        FWidget* ownerMenu = ownerItem->menuOwner;
                        if (ownerMenu == nullptr)
                            break;

                        if (ownerMenu->IsOfType<FMenuPopup>())
                        {
                            FMenuPopup* menuOwnerPopup = static_cast<FMenuPopup*>(ownerMenu);

                            if (!focusedWidget->ParentExistsRecursive(menuOwnerPopup))
                            {
                                menuOwnerPopup->ClosePopup();
                            }

                            ownerItem = menuOwnerPopup->ownerItem;
                        }
                        else if (ownerMenu->IsOfType<FMenuBar>())
                        {
                            ownerItem = nullptr;
                        }
                    }
                }
                else
                {
                    ClosePopup();
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

