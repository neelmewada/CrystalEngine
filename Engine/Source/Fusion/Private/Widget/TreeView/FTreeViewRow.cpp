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
            FAssignNew(FHorizontalStack, contentStack)
            .ContentHAlign(HAlign::Fill)
            .ContentVAlign(VAlign::Fill)
            .HAlign(HAlign::Fill)
            .VAlign(VAlign::Fill)
        );
    }
    
}

