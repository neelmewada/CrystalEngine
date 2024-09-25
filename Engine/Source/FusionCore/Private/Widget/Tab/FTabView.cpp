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
            .Name("TabViewContainerParent")
            (
                FAssignNew(FHorizontalStack, tabWell)
                .ContentHAlign(HAlign::Left)
                .HAlign(HAlign::Fill)
                .Name("TabWell"),

                FAssignNew(FStyledWidget, container)
                .FillRatio(1.0f)
                .HAlign(HAlign::Fill)
                .VAlign(VAlign::Fill)
                .Name("TabViewContainer")
            )
        );
        
        isConstructed = true;
    }

    bool FTabView::TryAddChild(FWidget* child)
    {
        if (!child || isConstructed)
        {
	        return false;
        }

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
            SelectTab(0);
        }
        else
        {
            ApplyStyle();
        }
    }

    void FTabView::SelectTab(int tabIndex)
    {
        if (tabIndex < GetTabItemCount())
        {
            activeTabIndex = tabIndex;

            for (int i = 0; i < tabItems.GetSize(); ++i)
            {
	            if (i == activeTabIndex)
	            {
                    tabItems[i]->itemFlags |= FTabItemFlags::Active;
	            }
                else
                {
                    tabItems[i]->itemFlags &= ~FTabItemFlags::Active;
                }
            }

            container->Child(*tabItems[activeTabIndex]->m_ContentWidget);

            ApplyStyle();
        }
    }

    void FTabView::SelectTab(FTabItem* tabItem)
    {
	    for (int i = 0; i < GetTabItemCount(); ++i)
	    {
		    if (GetTabItem(i) == tabItem)
		    {
                SelectTab(i);
			    return;
		    }
	    }
    }

}

