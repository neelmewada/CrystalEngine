#include "FusionCore.h"

namespace CE
{

    FMenuBar::FMenuBar()
    {

    }

    void FMenuBar::OnMenuItemHovered(FMenuItem* hoveredItem)
    {
        for (FMenuItem* menuItem : menuItems)
        {
	        if (menuItem != hoveredItem && menuItem->subMenu != nullptr)
	        {
                menuItem->subMenu->ClosePopup();
	        }
        }
    }

    void FMenuBar::Construct()
    {
        Super::Construct();

        Child(
            FAssignNew(FStackBox, container)
            .Direction(FStackBoxDirection::Horizontal)
            .ContentHAlign(HAlign::Left)
            .ContentVAlign(VAlign::Fill)
        );
    }
    
}

