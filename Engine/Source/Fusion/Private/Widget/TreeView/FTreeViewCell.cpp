#include "Fusion.h"

namespace CE
{

    FTreeViewCell::FTreeViewCell()
    {

    }

    void FTreeViewCell::Construct()
    {
        Super::Construct();

        FBrush downwardArrow = FBrush("/Engine/Resources/Icons/DownwardArrow");

        Child(
            FNew(FHorizontalStack)
            .ContentHAlign(HAlign::Left)
            .ContentVAlign(VAlign::Fill)
            .VAlign(VAlign::Fill)
            .HAlign(HAlign::Fill)
            .Padding(Vec4(1.5f, 2, 1.5f, 2) * 3)
            (
                FAssignNew(FImage, arrowIcon)
                .Background(downwardArrow)
                .Angle(90)
                .Enabled(false),

                FAssignNew(FLabel, label)
                .HAlign(HAlign::Left)
            )
        );
    }
    
}

