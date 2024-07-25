#include "FusionCore.h"

namespace CE
{

    FMenuBar::FMenuBar()
    {

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

