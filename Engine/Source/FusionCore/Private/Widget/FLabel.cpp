#include "FusionCore.h"

namespace CE
{

    FLabel::FLabel()
    {
        m_Foreground = Color::White();
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

        if (m_Text.IsEmpty())
        {
            intrinsicSize = painter->CalculateTextSize(" ", m_Font);
        }
        else
        {
            intrinsicSize = painter->CalculateTextSize(m_Text, m_Font);
        }

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

        painter->DrawText(m_Text, Vec2(), computedSize, m_WordWrap);

        if (m_Underline.GetStyle() != FPenStyle::None && m_Underline.GetColor().a > 0.001f && 
            !m_Text.IsEmpty())
        {
            FFontMetrics metrics = painter->GetFontMetrics(m_Font);
            f32 lineHeight = metrics.lineHeight * (f32)m_Font.GetFontSize();

        	painter->GetRenderer()->CalculateUnderlinePositions(underlineRects, m_Text, m_Font, computedSize.width, m_WordWrap);

            f32 startX = 0;
            f32 curX = NumericLimits<f32>::Min();
            f32 curY = 0;
            int curLine = 0;

            for (int i = 0; i < underlineRects.GetSize(); ++i)
            {
                underlineRects[i] = underlineRects[i].Translate(Vec2(0, m_Underline.GetThickness()));

                painter->SetPen(m_Underline);
                painter->DrawLine(underlineRects[i].min, underlineRects[i].max);
            }
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

    const CE::Name& FLabel::FontFamily()
    {
        return m_Font.GetFamily();
    }

    const int& FLabel::FontSize()
    {
        return m_Font.GetFontSize();
    }

    const bool& FLabel::Bold()
    {
        return m_Font.IsBold();
    }

    const bool& FLabel::Italic()
    {
        return m_Font.IsItalic();
    }

}

