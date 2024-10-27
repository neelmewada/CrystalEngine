#include "Fusion.h"

namespace CE
{

    FTreeViewRow::FTreeViewRow()
    {

    }

    FTreeViewCell* FTreeViewRow::GetCell(u32 index) const
    {
        return static_cast<FTreeViewCell*>(contentStack->GetChild(index));
    }

    void FTreeViewRow::AddCell(FTreeViewCell& cell)
    {
        contentStack->AddChild(&cell);
    }

    void FTreeViewRow::AddCell(FTreeViewCell* cell)
    {
        contentStack->AddChild(cell);
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

