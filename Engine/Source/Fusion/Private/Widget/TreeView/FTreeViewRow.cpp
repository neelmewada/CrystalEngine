#include "Fusion.h"

namespace CE
{

    FTreeViewRow::FTreeViewRow()
    {

    }

    void FTreeViewRow::Construct()
    {
        Super::Construct();

        Child(
            FNew(FHorizontalStack)
            .HAlign(HAlign::Fill)
            .VAlign(VAlign::Fill)
        );
    }
    
}

