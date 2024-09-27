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
            )
        );
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

    void FListView::RegenerateRows()
    {
        // Destroy previous FListItemWidget's

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

        //scrollBox->ClampTranslation();
        MarkLayoutDirty();
    }

    FListView::Self& FListView::GenerateRowDelegate(const GenerateRowCallback& callback)
    {
        m_GenerateRowDelegate = callback;
        RegenerateRows();
        return *this;
    }

}

