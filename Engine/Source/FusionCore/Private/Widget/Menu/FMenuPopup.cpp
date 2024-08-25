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

            if (focusEvent->GotFocus() != IsFocused())
            {
                if (focusEvent->LostFocus() && AutoClose())
                {
                    FWidget* focusedWidget = focusEvent->focusedWidget;

                    FMenuItem* ownerItem = this->ownerItem;

                    while (ownerItem != nullptr && ownerItem->subMenu != nullptr)
                    {
                        if (focusedWidget == nullptr)
                        {
                            ownerItem->subMenu->ClosePopup();
                        }

                        FWidget* ownerItemMenu = ownerItem->menuOwner;
                        if (ownerItemMenu == nullptr)
                            break;
                        
                        if (ownerItemMenu->IsOfType<FMenuPopup>())
                        {
                            FMenuPopup* ownerItemMenuPopup = static_cast<FMenuPopup*>(ownerItemMenu);
                            if (!focusedWidget->FocusParentExistsRecursive(ownerItemMenuPopup))
                            {
                                ownerItemMenuPopup->ClosePopup();
                            }
	                        ownerItem = ownerItemMenuPopup->ownerItem;
                        }
                        else
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

