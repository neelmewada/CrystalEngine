#include "Fusion.h"

namespace CE
{

    FTreeViewHeaderColumn::FTreeViewHeaderColumn()
    {

    }

    void FTreeViewHeaderColumn::Construct()
    {
        Super::Construct();

        Child(
            FNew(FHorizontalStack)
            .ContentHAlign(HAlign::Left)
            .HAlign(HAlign::Fill)
            .VAlign(VAlign::Fill)
            .Padding(Vec4(1.5f, 2, 1.5f, 2) * 3)
            (
                FAssignNew(FLabel, label)
                .FontSize(15)
                .HAlign(HAlign::Left)
            )
        );
    }
    
}

