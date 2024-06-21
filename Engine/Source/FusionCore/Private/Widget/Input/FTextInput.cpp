#include "FusionCore.h"

namespace CE
{

    FTextInput::FTextInput()
    {

    }

    void FTextInput::CalculateIntrinsicSize()
    {
        FFusionContext* context = GetContext();
        if (!context)
        {
            Super::CalculateIntrinsicSize();
            return;
        }

        FPainter* painter = context->GetPainter();
        if (!painter)
        {
            Super::CalculateIntrinsicSize();
            return;
        }

        intrinsicSize = painter->CalculateTextSize(m_Text, m_Font);
        intrinsicSize.width += m_Padding.left + m_Padding.right;
        intrinsicSize.height += m_Padding.top + m_Padding.bottom;
    }

    void FTextInput::OnPaintContent(FPainter* painter)
    {
        Super::OnPaintContent(painter);

        painter->SetPen(FPen(m_Foreground));
        painter->SetFont(m_Font);

        
    }

    void FTextInput::OnFusionPropertyModified(const CE::Name& propertyName)
    {
        Super::OnFusionPropertyModified(propertyName);

        static const CE::Name textName = "Text";

        if (propertyName == textName)
        {

        }
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

} // namespace CE
