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
                FAssignNew(FVerticalStack, content)
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
        // TODO: Regenerate all rows while keeping scroll position intact

        

        //scrollBox->ClampTranslation();
    }

    FListView::Self& FListView::OnGenerateRow(const GenerateRowCallback& callback)
    {
        m_OnGenerateRow = callback;
        RegenerateRows();
        return *this;
    }

}

