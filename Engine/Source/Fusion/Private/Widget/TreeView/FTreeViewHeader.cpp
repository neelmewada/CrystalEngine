#include "Fusion.h"

namespace CE
{

    FTreeViewHeader::FTreeViewHeader()
    {

    }

    FTreeViewHeaderColumn* FTreeViewHeader::GetHeader(u32 index)
    {
        return static_cast<FTreeViewHeaderColumn*>(contentStack->GetChild(index));
    }

    void FTreeViewHeader::Construct()
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

    FStyledWidget* FTreeViewHeader::CreateSeparator()
    {
        FStyledWidget* separator = nullptr;

        FAssignNew(FStyledWidget, separator)
        .Background(Color::Black())
        .Width(5)
        .VAlign(VAlign::Fill)
    	;

        if (treeView)
        {
            treeView->ApplyStyle();
        }

        return separator;
    }

}

