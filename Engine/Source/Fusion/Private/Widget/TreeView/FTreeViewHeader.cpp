#include "Fusion.h"

namespace CE
{

    FTreeViewHeader::FTreeViewHeader()
    {

    }

    u32 FTreeViewHeader::GetColumnCount() const
    {
        if (contentStack->GetChildCount() == 0)
            return 0;
        return contentStack->GetChildCount() / 2 + 1;
    }

    FTreeViewHeaderColumn* FTreeViewHeader::GetColumn(u32 index)
    {
        return static_cast<FTreeViewHeaderColumn*>(contentStack->GetChild(index * 2).Get());
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
        .Width(1)
        .Padding(Vec4())
        .VAlign(VAlign::Fill)
        .Name("Separator")
    	;

        if (treeView)
        {
            treeView->ApplyStyle();
        }

        return separator;
    }

}

