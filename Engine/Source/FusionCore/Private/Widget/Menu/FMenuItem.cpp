#include "FusionCore.h"

namespace CE
{

    FMenuItem::FMenuItem()
    {

    }

    void FMenuItem::Construct()
    {
	    Super::Construct();

        Child(
            FNew(FHorizontalStack)
            (
                FAssignNew(FLabel, label)
                .Text("")
                .FontSize(14)
            )
        );
    }

    void FMenuItem::HandleEvent(FEvent* event)
    {
        if (event->sender == this)
        {
            if (event->type == FEventType::MouseEnter && !isHovered)
            {
                isHovered = true;
                if (menuOwner)
                    menuOwner->ApplyStyle();
            }
            else if (event->type == FEventType::MouseLeave && isHovered)
            {
                isHovered = false;
                if (menuOwner)
                    menuOwner->ApplyStyle();
                if (subMenu && (!menuOwner || menuOwner->IsOfType<FMenuBar>()))
                {
                    
                }
            }
            else if (event->type == FEventType::MousePress)
            {
                if (subMenu)
                {
                    GetContext()->PushLocalPopup(subMenu, globalPosition + Vec2(0, computedSize.y));
                }
                else if (menuOwner)
                {
	                if (menuOwner->IsOfType<FMenuPopup>())
	                {
                        FMenuPopup* menuPopup = static_cast<FMenuPopup*>(menuOwner);
                        menuPopup->ClosePopup();

                        
	                }
                    else if (menuOwner->IsOfType<FMenuBar>())
                    {
	                    
                    }
                }

                m_OnClick();
            }
        }

	    Super::HandleEvent(event);
    }

    FMenuItem& FMenuItem::SubMenu(FMenuPopup& subMenu)
    {
        this->subMenu = &subMenu;
        return *this;
    }

}

