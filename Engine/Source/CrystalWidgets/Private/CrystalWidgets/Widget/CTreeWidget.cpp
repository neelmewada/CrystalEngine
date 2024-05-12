#include "CrystalWidgets.h"

namespace CE::Widgets
{

    CTreeWidget::CTreeWidget()
    {
        allowVerticalScroll = true;
        allowHorizontalScroll = false;
        clipChildren = true;

        AddDefaultBehavior<CScrollBehavior>();
    }

    CTreeWidget::~CTreeWidget()
    {
	    
    }

    bool CTreeWidget::IsSubWidgetAllowed(Class* subWidgetClass)
    {
        return subWidgetClass->IsSubclassOf<CTreeWidgetRow>();
    }

    void CTreeWidget::Construct()
    {
	    Super::Construct();
    }

    void CTreeWidget::OnSubobjectAttached(Object* subobject)
    {
        Super::OnSubobjectAttached(subobject);

        if (subobject->IsOfType<CTreeWidgetItem>())
        {
            CTreeWidgetItem* item = static_cast<CTreeWidgetItem*>(subobject);
            item->treeWidget = this;

            items.Add(item);
            UpdateRows();
        }
        else if (subobject->IsOfType<CTreeWidgetRow>())
        {
            CTreeWidgetRow* row = static_cast<CTreeWidgetRow*>(subobject);
            row->treeWidget = this;
        }
    }

    void CTreeWidget::OnSubobjectDetached(Object* subobject)
    {
	    Super::OnSubobjectDetached(subobject);

        if (subobject->IsOfType<CTreeWidgetItem>())
        {
            CTreeWidgetItem* item = static_cast<CTreeWidgetItem*>(subobject);
            
        }
        else if (subobject->IsOfType<CTreeWidgetRow>())
        {
            CTreeWidgetRow* row = static_cast<CTreeWidgetRow*>(subobject);
            row->treeWidget = nullptr;

            rows.Remove(row);
        }
    }

    void CTreeWidget::UpdateRows()
    {
        int rowCounter = 0;

        std::function<void(CTreeWidgetItem*, int)> visitor = [&](CTreeWidgetItem* item, int indentLevel)
            {
                if (!item)
                    return;
                CTreeWidgetRow* row = nullptr;
				
				while (rowCounter >= rows.GetSize())
				{
                    row = CreateObject<CTreeWidgetRow>(this, String("Row_") + rowCounter);
                    rows.Add(row);
				}
                
                row = rows[rowCounter];
                row->SetName(String("Row_") + rowCounter);
                rowCounter++;

                item->row = row;
                row->ownerItem = item;
				if (item->parent != row && item->parent != nullptr)
				{
                    item->parent->RemoveSubWidget(item);
				}
                row->AddSubWidget(item);
                row->rootPadding = Vec4(indentLevel * 15.0f, 0, 0, 0);

				if (item->IsExpanded())
				{
                    for (CTreeWidgetItem* child : item->children)
                    {
                        visitor(child, indentLevel + 1);
                    }
				}
            };

        for (CTreeWidgetItem* item : items)
        {
            visitor(item, 0);
        }

        for (int i = 0; i < rows.GetSize(); ++i)
        {
            rows[i]->SetAlternate(i % 2 != 0);
        }
    }

} // namespace CE::Widgets
