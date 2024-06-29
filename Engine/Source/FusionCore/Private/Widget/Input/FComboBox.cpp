#include "FusionCore.h"

namespace CE
{

    FComboBox::FComboBox()
    {
        m_Padding = Vec4(7.5f, 5, 10, 5);
    }

    void FComboBox::Construct()
    {
        Super::Construct();

        FBrush downwardArrow = FBrush("DownwardArrow");

        Child(
            FNew(FHorizontalStack)
            .ContentVAlign(VAlign::Center)
            (
                FAssignNew(FLabel, selectionText)
                .Text("[Select]")
                .FontSize(13)
                .Name("SelectionText")
                .Margin(Vec4(0, 0, 5, 0)),

                FNew(FImage)
                .Background(downwardArrow)
                .Width(10)
                .Height(10)
            )
        );
    }

    void FComboBox::HandleEvent(FEvent* event)
    {
        if (event->IsMouseEvent())
        {
            FMouseEvent* mouseEvent = static_cast<FMouseEvent*>(event);

            if (mouseEvent->type == FEventType::MouseEnter && event->sender == this)
            {
                if (!EnumHasFlag(state, FComboBoxState::Hovered))
                {
                    state |= FComboBoxState::Hovered;
                    ApplyStyle();
                }
                event->Consume(this);
            }
            else if (mouseEvent->type == FEventType::MouseLeave && event->sender == this)
            {
                if (EnumHasFlag(state, FComboBoxState::Hovered))
                {
                    state &= ~FComboBoxState::Hovered;
                    ApplyStyle();
                }
                event->Consume(this);
            }
            else if (mouseEvent->type == FEventType::MousePress && mouseEvent->buttons == MouseButtonMask::Left)
            {
                if (!EnumHasFlag(state, FComboBoxState::Pressed))
                {
                    state |= FComboBoxState::Pressed;
                    ApplyStyle();
                }
                event->Consume(this);
            }
            else if (mouseEvent->type == FEventType::MouseRelease && mouseEvent->buttons == MouseButtonMask::Left)
            {
                if (EnumHasFlag(state, FComboBoxState::Pressed))
                {
                    state &= ~FComboBoxState::Pressed;
                    ApplyStyle();

                    if (mouseEvent->isInside)
                    {
                        
                    }
                }
                event->Consume(this);
            }
        }

	    Super::HandleEvent(event);
    }
}

