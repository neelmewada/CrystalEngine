#include "EditorCore.h"

namespace CE::Editor
{

    NumericInputField::NumericInputField()
    {
        m_Padding = Vec4(10, 5, 10, 5);
        m_RangeMin = -NumericLimits<f32>::Infinity();
        m_RangeMax = NumericLimits<f32>::Infinity();
    }

    void NumericInputField::Construct()
    {
        Super::Construct();

        NumericType<f32>();

        Text("0");
    }

    void NumericInputField::OnFinishEdit()
    {
	    Super::OnFinishEdit();

        const String& text = Text();
        String lowerText = text.ToLower();

        if (!text.IsEmpty())
        {
            f32 value = 0;
            if (String::TryParse(text, value))
            {
                value = Math::Clamp(value, m_RangeMin, m_RangeMax);
                Text(String::Format("{}", value));
            }
            else
            {
                Text(String::Format("{}", Math::Clamp<f32>(0, m_RangeMin, m_RangeMax)));
            }
        }
    }

    void NumericInputField::OnPaint(FPainter* painter)
    {
	    Super::OnPaint(painter);

        if (!m_ColorTagVisible || m_ColorTag.a < 0.01f)
            return;

        constexpr f32 padding = 5.0f;
        constexpr f32 width = 2.0f;
        Vec2 pos = computedPosition + Vec2(padding, padding);
        Vec2 size = Vec2(width, computedSize.height - padding * 2);

        painter->SetBrush(m_ColorTag);
        painter->SetPen(FPen());
        painter->DrawRoundedRect(Rect::FromSize(pos, size), Vec4(2, 0, 0, 2));
    }

}

