#include "FusionCore.h"

namespace CE
{

    FTabView::FTabView()
    {

    }

    void FTabView::Construct()
    {
        Super::Construct();

        Child(
            FNew(FVerticalStack)
            .ContentHAlign(HAlign::Fill)
            (
                FAssignNew(FHorizontalStack, tabWell)
                .ContentHAlign(HAlign::Left)
                .HAlign(HAlign::Fill)
                .Name("TabWell"),

                FAssignNew(FStyledWidget, container)
                .HAlign(HAlign::Fill)
                .VAlign(VAlign::Fill)
            )
        );
        
        isConstructed = true;
    }

    bool FTabView::TryAddChild(FWidget* child)
    {
        if (!child || isConstructed)
            return false;

	    return Super::TryAddChild(child);
    }

    void FTabView::AddTabItem(FTabItem* item)
    {
        if (!item || !item->m_ContentWidget)
            return;

        item->tabView = this;
        tabWell->AddChild(item);
        tabItems.Add(item);

        if (tabItems.GetSize() == 1)
        {
            activeTabIndex = 0;
            item->itemFlags = FTabItemFlags::Active;
        }

        ApplyStyle();
    }

}

