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
            .Padding(Vec4(1, 1, 1, 1) * 10.0f)
            (
                FAssignNew(FLabel, label)
                .Text("")
                .FontSize(14)
            )
        );
    }

    FMenuItem& FMenuItem::SubMenu(FMenuPopup& subMenu)
    {
        this->subMenu = &subMenu;
        return *this;
    }

}

