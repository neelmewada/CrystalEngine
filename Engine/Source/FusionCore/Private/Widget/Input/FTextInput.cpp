#include "FusionCore.h"

namespace CE
{
    FTextInputLabel::FTextInputLabel()
    {
	    
    }

    FTextInput::FTextInput()
    {
        m_Padding = Vec4(7.5f, 5, 7.5f, 5);
    }

    void FTextInput::OnPaintContent(FPainter* painter)
    {
	    Super::OnPaintContent(painter);


    }

    void FTextInput::HandleEvent(FEvent* event)
    {
        if (event->IsMouseEvent() && event->sender == this)
        {
            FMouseEvent* mouseEvent = static_cast<FMouseEvent*>(event);

            if (mouseEvent->type == FEventType::MousePress)
            {
                
            }
        }

	    Super::HandleEvent(event);
    }

    void FTextInput::Construct()
    {
	    Super::Construct();

        Child(
            FNew(FHorizontalStack)
            .ContentVAlign(VAlign::Center)
            (
                FAssignNew(FCompoundWidget, leftSlot),

                FNew(FStyledWidget)
                .ClipShape(FRectangle())
                (
                    FAssignNew(FTextInputLabel, inputLabel)
                    .Text("")
                    .FontSize(13)
                    .Foreground(Color::White())
                    .Name("TextInputLabel")
                )
            )
        );
    }

    void FTextInput::OnFusionPropertyModified(const CE::Name& propertyName)
    {
        Super::OnFusionPropertyModified(propertyName);

        static const CE::Name TextName = "Text";

        if (propertyName == TextName)
        {
            
        }
    }

    void FTextInput::SetEditing(bool edit)
    {
        if (IsEditing() == edit)
            return;

        if (edit)
        {
            state |= FTextInputState::Editing;
        }
        else
        {
            state &= ~FTextInputState::Editing;
        }

        ApplyStyle();
    }

    FTextInput& FTextInput::Text(const String& value)
    {
        inputLabel->Text(value);
        return *this;
    }

    FTextInput& FTextInput::Foreground(const Color& value)
    {
        inputLabel->Foreground(value);
        return *this;
    }

    FTextInput& FTextInput::FontFamily(const CE::Name& fontFamily)
    {
        FFont copy = m_Font;
        copy.SetFamily(fontFamily);
        return Font(copy);
    }

    FTextInput& FTextInput::FontSize(int fontSize)
    {
        FFont copy = m_Font;
        copy.SetFontSize(fontSize);
        return Font(copy);
    }

    FTextInput& FTextInput::Bold(bool bold)
    {
        FFont copy = m_Font;
        copy.SetBold(bold);
        return Font(copy);
    }

    FTextInput& FTextInput::Italic(bool italic)
    {
        FFont copy = m_Font;
        copy.SetItalic(italic);
        return Font(copy);
    }

    FTextInput& FTextInput::LeftSlot(FWidget& content)
    {
        leftSlot->Child(content);
        return *this;
    }

} // namespace CE
