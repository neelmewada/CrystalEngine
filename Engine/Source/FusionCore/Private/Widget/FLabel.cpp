#include "FusionCore.h"

namespace CE
{

    FLabel::FLabel()
    {

    }

    void FLabel::CalculateIntrinsicSize()
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

        ApplyIntrinsicSizeConstraints();
    }

    void FLabel::OnPaint(FPainter* painter)
    {
	    Super::OnPaint(painter);

        painter->SetFont(m_Font);
        painter->SetPen(FPen(m_Foreground));
        painter->SetBrush(FBrush());

        bool transformChanged = false;

        if (m_Translation.GetSqrMagnitude() > 0 || abs(m_Angle) > 0 || m_Scale.GetSqrMagnitude() != 1)
        {
            painter->SetItemTransform(Matrix4x4::Translation(m_Translation) * Matrix4x4::Angle(m_Angle) * Matrix4x4::Scale(m_Scale));
            transformChanged = true;
        }

        painter->DrawText(m_Text, computedPosition, computedSize, m_WordWrap);

        if (transformChanged)
        {
            painter->SetItemTransform(Matrix4x4::Identity());
        }
    }

    FLabel::Self& FLabel::FontFamily(const CE::Name& fontFamily)
    {
        FFont copy = m_Font;
        copy.SetFamily(fontFamily);
        return Font(copy);
    }

    FLabel::Self& FLabel::FontSize(int fontSize)
    {
        FFont copy = m_Font;
        copy.SetFontSize(fontSize);
        return Font(copy);
    }

    FLabel::Self& FLabel::Bold(bool bold)
    {
        FFont copy = m_Font;
        copy.SetBold(bold);
        return Font(copy);
    }

    FLabel::Self& FLabel::Italic(bool italic)
    {
        FFont copy = m_Font;
        copy.SetItalic(italic);
        return Font(copy);
    }

}

