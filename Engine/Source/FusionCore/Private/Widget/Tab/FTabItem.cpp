#include "FusionCore.h"

namespace CE
{

    FTabItem::FTabItem()
    {
        m_Padding = Vec4(1, 1, 1, 1) * 2.5f;
    }

    FLabelTabItem::FLabelTabItem()
    {

    }

    void FLabelTabItem::Construct()
    {
	    Super::Construct();

        Child(
            FAssignNew(FLabel, label)
            .FontSize(14)
            .Foreground(Color::White())
        );
    }

}

