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

    void CListWidget::Select(CListWidgetItem* item)
    {
        SetNeedsStyle();
        SetNeedsPaint();

        if (!item || item->owner != this)
        {
            this->selectedItem = nullptr;
	        return;
        }

        this->selectedItem = item;

        for (CListWidgetItem* listItem : items)
        {
	        if (listItem == selectedItem)
	        {
                listItem->stateFlags |= CStateFlag::Active;
	        }
            else
            {
                listItem->stateFlags &= ~CStateFlag::Active;
            }
        }
    }

    void CListWidget::OnSubobjectAttached(Object* object)
    {
	    Super::OnSubobjectAttached(object);
        
        if (!object)
            return;

        if (object->IsOfType<CListWidgetItem>())
        {
            CListWidgetItem* item = static_cast<CListWidgetItem*>(object);
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
