#include "Fusion.h"

namespace CE
{
	FListView::FListView()
	{

	}

    void FListView::Construct()
    {
        Super::Construct();

        Child(
            FAssignNew(FScrollBox, scrollBox)
            .VerticalScroll(true)
            .HorizontalScroll(false)
            .Child(
                FAssignNew(FListViewContainer, content)
                .HAlign(HAlign::Fill)
                .Padding(Vec4(1, 1, 1, 1) * 10)
            )
            .HAlign(HAlign::Fill)
            .Name("ListViewScrollBox")
        );
    }

    void FListView::OnPostComputeLayout()
    {
	    Super::OnPostComputeLayout();

    }

    void FListView::OnFusionPropertyModified(const CE::Name& propertyName)
    {
	    Super::OnFusionPropertyModified(propertyName);

        thread_local const CE::Name ItemListName = "ItemList";

        if (propertyName == ItemListName)
        {
            RegenerateRows();
        }
    }

    void FListView::OnItemSelected(FListItemWidget* selectedItem)
    {
        if (m_SelectionMode == FSelectionMode::None)
            return;

        for (FListItemWidget* item : itemWidgets)
        {
	        if (item != selectedItem && item->IsSelected())
	        {
                item->itemState &= ~FListItemState::Selected;
	        }
            else if (item == selectedItem && !item->IsSelected())
            {
                item->itemState |= FListItemState::Selected;
            }
        }

        ApplyStyle();
    }

    void FListView::RegenerateRows()
    {
        // Destroy previous FListItemWidget's

        // TODO: Optimize row generation by using recycling, culling, etc

        while (content->GetChildCount() > 0)
        {
            content->GetChild(0)->Destroy();
        }

        itemWidgets.Clear();

        if (m_GenerateRowDelegate.IsValid())
        {
            for (FListItem* item : m_ItemList)
            {
                FListItemWidget& itemWidget = m_GenerateRowDelegate(item, this);
                itemWidget.listView = this;
                content->AddChild(&itemWidget);
                itemWidgets.Add(&itemWidget);
            }
        }

        scrollBox->ClampTranslation();
        MarkLayoutDirty();
    }

    FListView::Self& FListView::GenerateRowDelegate(const GenerateRowCallback& callback)
    {
        m_GenerateRowDelegate = callback;
        RegenerateRows();
        return *this;
    }

}

