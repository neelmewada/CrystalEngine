#include "FusionCore.h"

namespace CE
{

    FComboBoxItem::FComboBoxItem()
    {
        m_Padding = Vec4(1, 1, 1, 1) * 2.0f;
    }

    void FComboBoxItem::Construct()
    {
        Super::Construct();
        
        Child(
            FAssignNew(FHorizontalStack, contentBox)
            .ContentHAlign(HAlign::Left)
            .ContentVAlign(VAlign::Center)
            (
                FAssignNew(FLabel, label)
                .Text("")
            )
        );
        
    }
    
}

