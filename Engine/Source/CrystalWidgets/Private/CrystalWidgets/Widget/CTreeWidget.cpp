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

    void CTreeWidget::SetSelectionMode(CItemSelectionMode selectionMode)
    {
        this->selectionMode = selectionMode;

        UpdateSelection();
    }

    void CTreeWidget::Select(CTreeWidgetItem* item, bool selectAdditive)
    {
        if (selectionMode == CItemSelectionMode::NoSelection)
        {
            UpdateSelection();
            return;
        }

        if (!selectAdditive || selectionMode == CItemSelectionMode::SingleSelection)
            selectedItems.Clear();

        selectedItems.Add(item);

        UpdateSelection();
    }

    void CTreeWidget::RemoveItem(CTreeWidgetItem* item)
    {
        if (item->row)
        {
            item->row->RemoveSubWidget(item);
        }

        selectedItems.Remove(item);
        items.Remove(item);
        item->treeWidget = nullptr;
        item->row = nullptr;

        item->Destroy();
        
        UpdateRows();
        UpdateSelection();
    }

    void CTreeWidget::RemoveAllItems()
    {
        selectedItems.Clear();

        for (int i = items.GetSize() - 1; i >= 0; --i)
        {
            CTreeWidgetItem* item = items[i];
            if (item->row)
            {
                item->row->RemoveSubWidget(item);
            }

            items.RemoveAt(i);
            item->treeWidget = nullptr;
            item->row = nullptr;

            item->Destroy();
        }

        for (int i = rows.GetSize() - 1; i >= 0; --i)
        {
            rows[i]->Destroy();
        }

        UpdateRows();
        UpdateSelection();
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

            if (!items.Exists(item))
	        {
		        items.Add(item);
            	UpdateRows();
	        }
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

                for (int i = row->attachedWidgets.GetSize() - 1; i >= 0; --i)
                {
                    CWidget* widget = row->attachedWidgets[i];
                    if (!widget)
                        continue;
                    if (widget->IsOfType<CTreeWidgetItem>())
                    {
                        row->RemoveSubWidget(widget);
                    }
                }

				if (item->parent != row && item->parent != nullptr)
				{
                    item->parent->RemoveSubWidget(item);
				}
				
                row->AddSubWidget(item);
                row->rootPadding = Vec4(indentLevel * 15.0f, 0, 0, 0);

                item->arrowIcon->SetVisible(item->children.NonEmpty());

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

        for (int i = rows.GetSize() - 1; i >= 0; --i)
        {
	        if (rows[i]->GetSubWidgetCount() == 0 || rows[i]->ownerItem == nullptr || i >= rowCounter)
	        {
                CTreeWidgetRow* row = rows[i];

                for (int j = row->attachedWidgets.GetSize() - 1; j >= 0; --j)
                {
	                if (row->attachedWidgets[j]->IsOfType<CTreeWidgetItem>())
	                {
                        row->RemoveSubWidget(row->attachedWidgets[j]);
	                }
                }

                row->Destroy();
                rows.Remove(row);
	        }
        }
    }

    void CTreeWidget::UpdateSelection()
    {
        switch (selectionMode)
        {
        case CItemSelectionMode::SingleSelection:
            if (selectedItems.GetSize() > 1)
            {
                CTreeWidgetItem* item = *selectedItems.begin();
                selectedItems.Clear();
                selectedItems.Add(item);
                SetNeedsPaint();
            }
            break;
        case CItemSelectionMode::NoSelection:
            if (!selectedItems.IsEmpty())
            {
                selectedItems.Clear();
                SetNeedsPaint();
            }
            break;
        default:
            break;
        }

        std::function<void(CTreeWidgetItem*)> visitor = [&](CTreeWidgetItem* treeWidgetItem)
            {
                bool isSelected = selectedItems.Exists(treeWidgetItem);
                CTreeWidgetRow* row = treeWidgetItem->row;

                if (row && isSelected && !EnumHasFlag(row->stateFlags, CStateFlag::Active))
                {
                    row->stateFlags |= CStateFlag::Active;

                    row->SetNeedsStyle();
                    row->SetNeedsPaint();
                }
                else if (row && !isSelected && EnumHasFlag(row->stateFlags, CStateFlag::Active))
                {
                    row->stateFlags &= ~CStateFlag::Active;

                    row->SetNeedsStyle();
                    row->SetNeedsPaint();
                }

				for (CTreeWidgetItem* child : treeWidgetItem->children)
				{
                    visitor(child);
				}
            };

        for (CTreeWidgetItem* treeWidgetItem : items)
        {
            visitor(treeWidgetItem);
        }

        emit OnSelectionChanged(this);
    }

} // namespace CE::Widgets
