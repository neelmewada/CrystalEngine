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

        if (GetName() == "TitleLabel")
        {
            painter->SetPen(FPen());
            painter->SetBrush(Color::Red());

            painter->DrawRect(Rect::FromSize(computedPosition, computedSize));
        }

        painter->SetFont(m_Font);
        painter->SetPen(FPen(m_Foreground));
        painter->SetBrush(FBrush());

        painter->DrawText(m_Text, computedPosition, computedSize, m_WordWrap);

        if (m_Underline.GetStyle() != FPenStyle::None && m_Underline.GetColor().a > 0.001f && 
            !m_Text.IsEmpty())
        {
            FFontMetrics metrics = painter->GetFontMetrics(m_Font);
            f32 lineHeight = metrics.lineHeight * (f32)m_Font.GetFontSize();

        	painter->CalculateCharacterOffsets(charRects, m_Text, m_Font, computedSize.width, m_WordWrap);

            f32 startX = 0;
            f32 curX = NumericLimits<f32>::Min();
            f32 curY = 0;
            int curLine = 0;

            for (int i = 0; i < charRects.GetSize(); ++i)
            {
	            if (i == 0 || startX < -1000)
	            {
                    startX = charRects[0].min.x;

                    curX = charRects[0].max.x;
                    curY = charRects[0].min.y;
	            }
                else
                {
	                if (charRects[i].max.x < curX || i == (int)charRects.GetSize() - 1) // New line OR EOL
	                {
                        if (i == (int)charRects.GetSize() - 1)
                        {
                            curX = charRects[i].max.x;
                            curY = charRects[i].min.y;
                        }

                        painter->SetPen(m_Underline);
                        painter->DrawLine(computedPosition + Vec2(startX, curY + 1),
                            computedPosition + Vec2(curX, curY + 1));

                        startX = charRects[i].min.x;
                        curX = charRects[i].max.x;
                        curY = charRects[i].min.y;
	                }
                    else
                    {
                        curX = charRects[i].max.x;
                        curY = charRects[i].min.y;
                    }
                }
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

