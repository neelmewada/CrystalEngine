#include "EditorCore.h"

namespace CE::Editor
{

    NumericInputField::NumericInputField()
    {
        //sm_Padding = Vec4(10, 4, 10, 4);
        m_RangeMin = -NumericLimits<f32>::Infinity();
        m_RangeMax = NumericLimits<f32>::Infinity();
    }

    void NumericInputField::Construct()
    {
        Super::Construct();

        Child(
			FAssignNew(FTextInput, input)
            .OnTextEditingFinished(FUNCTION_BINDING(this, OnFinishEdit))
            .Padding(Vec4(10, 4, 8, 4))
        );

        NumericType<f32>();
        input->Text("0");
    }

    void NumericInputField::OnFinishEdit(FTextInput*)
    {
        const String& text = input->Text();
        String lowerText = text.ToLower();

        if (!text.IsEmpty())
        {
            f32 value = 0;
            if (String::TryParse(text, value))
            {
                value = Math::Clamp(value, m_RangeMin, m_RangeMax);
                input->Text(String::Format("{}", value));
            }
            else
            {
                input->Text(String::Format("{}", Math::Clamp<f32>(0, m_RangeMin, m_RangeMax)));
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

    NumericInputField::Self& NumericInputField::NumericType(TypeId type)
    {
        numericType = type;

        if (type == TYPEID(f32))
        {
            input->Validator(FNumericInputValidator<f32>);
        }
        else if (type == TYPEID(f64))
        {
            input->Validator(FNumericInputValidator<f64>);
        }
        else if (type == TYPEID(u8))
        {
            input->Validator(FNumericInputValidator<u8>);
        }
        else if (type == TYPEID(s8))
        {
            input->Validator(FNumericInputValidator<s8>);
        }
        else if (type == TYPEID(u16))
        {
            input->Validator(FNumericInputValidator<u16>);
        }
        else if (type == TYPEID(s16))
        {
            input->Validator(FNumericInputValidator<s16>);
        }
        else if (type == TYPEID(u32))
        {
            input->Validator(FNumericInputValidator<u32>);
        }
        else if (type == TYPEID(s32))
        {
            input->Validator(FNumericInputValidator<s32>);
        }
        else if (type == TYPEID(u64))
        {
            input->Validator(FNumericInputValidator<u64>);
        }
        else if (type == TYPEID(s64))
        {
            input->Validator(FNumericInputValidator<s64>);
        }

        return *this;
    }
}

