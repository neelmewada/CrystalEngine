#include "Fusion.h"

namespace CE
{

    FTreeViewCell::FTreeViewCell()
    {

    }

    void FTreeViewCell::Construct()
    {
        Super::Construct();

        Child(
            FNew(FHorizontalStack)
            .ContentHAlign(HAlign::Left)
            .ContentVAlign(VAlign::Fill)
            .VAlign(VAlign::Fill)
            .HAlign(HAlign::Fill)
            (
                FAssignNew(FLabel, label)
                .HAlign(HAlign::Left)
            )
        );
    }
    
}

