#include "FusionCore.h"

namespace CE
{

    FComboBox::FComboBox()
    {
        m_Padding = Vec4(7.5f, 5, 10, 5);
    }

    void FComboBox::ApplyStyle()
    {
	    Super::ApplyStyle();

        ItemStyle(m_ItemStyle);
    }

    void FComboBox::SelectItem(FComboBoxItem* item)
    {
        selectedItem = item;
        selectionText->Text(selectedItem->Text());

        for (int i = 0; i < popupContent->GetChildCount(); ++i)
        {
            FWidget* child = popupContent->GetChild(i);
            if (!child->IsOfType<FComboBoxItem>())
                continue;

            FComboBoxItem* childItem = static_cast<FComboBoxItem*>(child);
            if (childItem == item)
            {
                childItem->state |= FComboBoxItemState::Selected;
            }
            else
            {
                childItem->state &= ~FComboBoxItemState::Selected;
            }
        }

        popupMenu->ClosePopup();

        ApplyStyle();
    }

    void FComboBox::SelectItem(int index)
    {
        if (index < 0 || index >= popupContent->GetChildCount())
            return;

        FWidget* child = popupContent->GetChild(index);
        if (!child->IsOfType<FComboBox>())
            return;

        SelectItem(static_cast<FComboBoxItem*>(child));
    }

    void FComboBox::AddItem(FComboBoxItem& item)
    {
        item.comboBox = this;

        if (m_ItemStyle)
        {
	        item.Style(m_ItemStyle);
        }

        popupMenu->Content(item);
    }

    void FComboBox::AddItem(const String& text)
    {
        FComboBoxItem& item = FNewOwned(FComboBoxItem, popupContent);
        AddItem(item);
        item.Text(text);
    }

    void FComboBox::Construct()
    {
        Super::Construct();

        FBrush downwardArrow = FBrush("DownwardArrow");

        FAssignNew(FComboBoxPopup, popupMenu)
    		.BlockInteraction(false)
    		.AutoClose(true)
    		.MinWidth(60)
    		.MinHeight(30);

        popupContent = popupMenu->GetContentStack();

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

                    if (mouseEvent->isInside)
                    {
                        Vec2 popupPosition = globalPosition;
                        popupPosition.y += computedSize.height;
                        popupMenu->MinWidth(GetComputedSize().width);

                        GetContext()->PushLocalPopup(popupMenu, popupPosition);
                    }

                    ApplyStyle();
                }
                event->Consume(this);
            }
        }

	    Super::HandleEvent(event);
    }

    FComboBox& FComboBox::ItemStyle(FComboBoxItemStyle* value)
    {
        m_ItemStyle = value;
        if (!popupContent)
            return *this;

        for (int i = 0; i < popupContent->GetChildCount(); ++i)
        {
            FWidget* child = popupContent->GetChild(i);
            child->Style(m_ItemStyle);
        }

        return *this;
    }

    FComboBox& FComboBox::ItemStyle(FStyleSet* styleSet, const CE::Name& styleKey)
    {
        if (!styleSet)
            return *this;

        FStyle* style = styleSet->FindStyle(styleKey);
        if (!style || !style->IsOfType<FComboBoxItemStyle>())
            return *this;

        return ItemStyle(static_cast<FComboBoxItemStyle*>(style));
    }

}

