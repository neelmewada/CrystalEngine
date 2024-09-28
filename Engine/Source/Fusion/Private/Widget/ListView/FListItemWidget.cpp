#include "Fusion.h"

namespace CE
{

    FListItemWidget::FListItemWidget()
    {

    }

    void FListItemWidget::Select()
    {
        if (!listView)
            return;

        listView->OnItemSelected(this);
    }

    void FListItemWidget::HandleEvent(FEvent* event)
    {
        if (event->IsMouseEvent() && listView && listView->SelectionMode() != FSelectionMode::None)
        {
            FMouseEvent* mouseEvent = static_cast<FMouseEvent*>(event);

            if (mouseEvent->type == FEventType::MouseEnter && !IsHovered() && !IsSelected())
            {
                itemState |= FListItemState::Hovered;

                ApplyStyle();
            }
            else if (mouseEvent->type == FEventType::MouseLeave && IsHovered() && !IsSelected())
            {
                itemState &= ~FListItemState::Hovered;

                ApplyStyle();
            }
        }

	    Super::HandleEvent(event);
    }

}

