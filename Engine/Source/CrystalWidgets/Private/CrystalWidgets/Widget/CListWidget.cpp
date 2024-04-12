#include "CrystalWidgets.h"

namespace CE::Widgets
{

    CListWidget::CListWidget()
    {
        receiveMouseEvents = true;
        receiveKeyEvents = true;

        allowVerticalScroll = true;
        allowHorizontalScroll = false;
    }

    CListWidget::~CListWidget()
    {
	    
    }

    bool CListWidget::IsSubWidgetAllowed(Class* subWidgetClass)
    {
        return subWidgetClass->IsSubclassOf<CListWidgetItem>();
    }

    void CListWidget::Select(CListWidgetItem* item, bool additive)
    {
        if (item == nullptr)
        {
            selectedItems.Clear();
            UpdateSelectionState();
            return;
        }

        if (item->owner != this)
        {
	        return;
        }

        if (selectionMode == CItemSelectionMode::NoSelection)
        {
            if (selectedItems.NonEmpty())
            {
                selectedItems.Clear();
                UpdateSelectionState();
            }
        }
        else if (selectionMode == CItemSelectionMode::SingleSelection)
        {
	        if (selectedItems.NonEmpty())
	        {
                selectedItems.Clear();
	        }
            selectedItems.Add(item);
            UpdateSelectionState();
        }
        else
        {
	        if (!additive)
	        {
                selectedItems.Clear();
	        }

            selectedItems.Add(item);
            UpdateSelectionState();
        }
    }

    void CListWidget::Select(int index, bool additive)
    {
        if (index < 0 || index >= items.GetSize())
            return;

        Select(items[index], additive);
    }

    void CListWidget::SetSelectionMode(CItemSelectionMode mode)
    {
        this->selectionMode = mode;

        if (selectionMode == CItemSelectionMode::NoSelection)
        {
            selectedItems.Clear();
        }
        else if (selectionMode == CItemSelectionMode::SingleSelection)
        {
	        while (selectedItems.GetSize() > 1)
	        {
                selectedItems.RemoveAt(0);
	        }
        }

        UpdateSelectionState();
    }

    void CListWidget::UpdateSelectionState()
    {
        for (int i = 0; i < items.GetSize(); i++)
        {
            CListWidgetItem* listItem = items[i];
            listItem->index = i;
            if (selectedItems.Exists(listItem))
            {
                listItem->stateFlags |= CStateFlag::Active;
            }
            else
            {
                listItem->stateFlags &= ~CStateFlag::Active;
            }
        }

        SetNeedsStyle();
        SetNeedsPaint();
    }

    void CListWidget::OnItemClicked(CListWidgetItem* item, KeyModifier modifiers)
    {
        Select(item, EnumHasAnyFlags(modifiers, KeyModifier::Ctrl));
    }

    void CListWidget::OnSubobjectAttached(Object* object)
    {
	    Super::OnSubobjectAttached(object);
        
        if (!object)
            return;

        if (object->IsOfType<CListWidgetItem>())
        {
            CListWidgetItem* item = static_cast<CListWidgetItem*>(object);
            item->index = items.GetSize();
            items.Add(item);
            item->owner = this;
        }
    }

    void CListWidget::OnSubobjectDetached(Object* object)
    {
        Super::OnSubobjectDetached(object);

        if (!object)
            return;

        if (object->IsOfType<CListWidgetItem>())
        {
            CListWidgetItem* item = static_cast<CListWidgetItem*>(object);
            item->index = -1;
            items.Remove(item);
            item->owner = nullptr;
        }
    }

    void CListWidget::HandleEvent(CEvent* event)
    {
        if (event->IsMouseEvent())
        {
            CMouseEvent* mouseEvent = static_cast<CMouseEvent*>(event);

            if (event->type == CEventType::MousePress)
            {
                CWidget* sender = event->sender;

                
            }
        }

	    Super::HandleEvent(event);
    }

} // namespace CE::Widgets
