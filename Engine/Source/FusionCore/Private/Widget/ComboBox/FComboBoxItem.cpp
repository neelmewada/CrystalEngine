#include "FusionCore.h"

namespace CE
{

    FComboBoxItem::FComboBoxItem()
    {
        m_Padding = Vec4(2, 1, 2, 1) * 2.0f;
    }

    void FComboBoxItem::Construct()
    {
        Super::Construct();
        
        Child(
            FAssignNew(FHorizontalStack, contentBox)
            .ContentHAlign(HAlign::Fill)
            .ContentVAlign(VAlign::Center)
            (
                FAssignNew(FLabel, label)
                .FontSize(13)
                .Text("")
            )
        );
        
    }

    void FComboBoxItem::HandleEvent(FEvent* event)
    {
        if (event->IsMouseEvent())
        {
            FMouseEvent* mouseEvent = static_cast<FMouseEvent*>(event);

            if (mouseEvent->type == FEventType::MouseEnter && event->sender == this)
            {
                if (!EnumHasFlag(state, FComboBoxItemState::Hovered))
                {
                    state |= FComboBoxItemState::Hovered;
                    ApplyStyle();
                }
                event->Consume(this);
            }
            else if (mouseEvent->type == FEventType::MouseLeave && event->sender == this)
            {
                if (EnumHasFlag(state, FComboBoxItemState::Hovered))
                {
                    state &= ~FComboBoxItemState::Hovered;
                    ApplyStyle();
                }
                event->Consume(this);
            }
            else if (mouseEvent->type == FEventType::MousePress && mouseEvent->buttons == MouseButtonMask::Left && event->sender == this)
            {
                comboBox->SelectItemInternal(this);
                ApplyStyle();
                event->Consume(this);
            }
        }

	    Super::HandleEvent(event);
    }

}

