#include "FusionCore.h"

namespace CE
{

    FComboBox::FComboBox()
    {
        m_Padding = Vec4(7.5f, 5, 10, 5);
    }

    void FComboBox::Construct()
    {
        Super::Construct();

        FBrush downwardArrow = FBrush("DownwardArrow");

        Child(
            FNew(FHorizontalStack)
            .ContentVAlign(VAlign::Center)
            (
                FAssignNew(FLabel, selectionText)
                .Text("[Select]")
                .FontSize(13)
                .Name("SelectionText")
                .Margin(Vec4(0, 0, 5, 0)),

                FNew(FImage)
                .Background(downwardArrow)
                .Width(10)
                .Height(10)
            )
        );
    }
    
}

