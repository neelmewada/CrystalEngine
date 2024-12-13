#include "EditorCore.h"

namespace CE::Editor
{

    NumericEditorField::NumericEditorField()
    {
    }

    void NumericEditorField::Construct()
    {
        Super::Construct();

        Child(
			FAssignNew(FTextInput, input)
            .OnTextEditingFinished(FUNCTION_BINDING(this, OnFinishEdit))
            .OnTextEdited(FUNCTION_BINDING(this, OnTextFieldEdited))
            .Padding(Vec4(11, 4, 8, 4))
        );

        NumericType<f32>();
        input->Text("0");
    }

    EditorField& NumericEditorField::FixedInputWidth(f32 width)
    {
        input->Width(width);
        return *this;
    }

    void NumericEditorField::HandleEvent(FEvent* event)
    {
        if (event->IsMouseEvent() && event->sender == this)
        {
            FMouseEvent* mouseEvent = static_cast<FMouseEvent*>(event);

            if (event->type == FEventType::MouseEnter)
            {
                isCursorPushed = true;
                FusionApplication::Get()->PushCursor(SystemCursor::SizeHorizontal);

                input->SetHoveredInternal(true);
                event->Consume(this);
            }
            else if (event->type == FEventType::MouseMove)
            {
                event->Consume(this);
            }
            else if (event->type == FEventType::MouseLeave)
            {
                if (isCursorPushed && !isDragging)
                {
                    isCursorPushed = false;
                    FusionApplication::Get()->PopCursor();
                }

                input->SetHoveredInternal(false);

                event->Consume(this);
            }
            else if (event->type == FEventType::MousePress)
            {
                event->Consume(this);

                if (mouseEvent->isDoubleClick)
                {
                    input->StartEditing(true);
                }
            }
            else if (event->type == FEventType::DragBegin)
            {
                FDragEvent* drag = static_cast<FDragEvent*>(mouseEvent);

                if (!input->IsEditing())
                {
	                
                }
            }
            else if (event->type == FEventType::DragMove)
            {
                FDragEvent* drag = static_cast<FDragEvent*>(mouseEvent);

            }
            else if (event->type == FEventType::DragEnd)
            {
                FDragEvent* drag = static_cast<FDragEvent*>(mouseEvent);

            }
        }

	    Super::HandleEvent(event);
    }

    FWidget* NumericEditorField::HitTest(Vec2 localMousePos)
    {
	    FWidget* thisHitTest = Super::HitTest(localMousePos);
        if (thisHitTest != nullptr && !input->IsEditing())
        {
            return this;
        }
        return thisHitTest;
    }

#define FIELD_SET_IF(type)\
	if (fieldDeclId == TYPEID(type))\
	{\
		type value = 0;\
		if (String::TryParse(text, value))\
		{\
			field->SetFieldValue<type>(instances[0], value);\
            target->OnFieldEdited(field->GetName());\
        }\
	}

    void NumericEditorField::OnFinishEdit(FTextInput*)
    {
        const String& text = input->Text();
        String lowerText = text.ToLower();

        if (!text.IsEmpty())
        {
            f32 value = 0;
            if (String::TryParse(text, value))
            {
                input->Text(String::Format("{}", value));
            }
            else
            {
                input->Text("0");
            }
        }
        else
        {
            input->Text("0");
        }

        m_OnTextEditingFinished(this);
    }

    void NumericEditorField::OnTextFieldEdited(FTextInput*)
    {
        m_OnTextEdited(this);

        if (!IsBound())
            return;

        const String& text = input->Text();

        Object* target = targets[0];

        TypeId fieldDeclId = field->GetDeclarationTypeId();

        FIELD_SET_IF(u8)
		else FIELD_SET_IF(s8)
        else FIELD_SET_IF(u16)
        else FIELD_SET_IF(s16)
        else FIELD_SET_IF(u32)
        else FIELD_SET_IF(s32)
        else FIELD_SET_IF(u64)
        else FIELD_SET_IF(s64)
        else FIELD_SET_IF(f32)
        else FIELD_SET_IF(f64)
    }

    void NumericEditorField::OnPaint(FPainter* painter)
    {
	    Super::OnPaint(painter);

        if (!m_ColorTagVisible || m_ColorTag.a < 0.01f)
            return;

        constexpr f32 padding = 5.0f;
        constexpr f32 width = 2.0f;
        Vec2 pos = Vec2(padding, padding);
        Vec2 size = Vec2(width, computedSize.height - padding * 2);

        painter->SetBrush(m_ColorTag);
        painter->SetPen(FPen());
        painter->DrawRoundedRect(Rect::FromSize(pos, size), Vec4(2, 0, 0, 2));
    }

    bool NumericEditorField::CanBind(FieldType* field)
    {
        return field->IsNumericField();
    }

    void NumericEditorField::UpdateValue()
    {
        if (!IsBound())
            return;

        TypeId fieldDeclId = field->GetDeclarationTypeId();

        NumericType(fieldDeclId);

        if (fieldDeclId == TYPEID(u8))
        {
            u8 value = field->GetFieldValue<u8>(instances[0]);
            Text(String::Format("{}", value));
        }
        else if (fieldDeclId == TYPEID(s8))
        {
            s8 value = field->GetFieldValue<s8>(instances[0]);
            Text(String::Format("{}", value));
        }
        else if (fieldDeclId == TYPEID(u16))
        {
            u16 value = field->GetFieldValue<u16>(instances[0]);
            Text(String::Format("{}", value));
        }
        else if (fieldDeclId == TYPEID(s16))
        {
            s16 value = field->GetFieldValue<s16>(instances[0]);
            Text(String::Format("{}", value));
        }
        else if (fieldDeclId == TYPEID(u32))
        {
            u32 value = field->GetFieldValue<u32>(instances[0]);
            Text(String::Format("{}", value));
        }
        else if (fieldDeclId == TYPEID(s32))
        {
            s32 value = field->GetFieldValue<s32>(instances[0]);
            Text(String::Format("{}", value));
        }
        else if (fieldDeclId == TYPEID(u64))
        {
            u64 value = field->GetFieldValue<u64>(instances[0]);
            Text(String::Format("{}", value));
        }
        else if (fieldDeclId == TYPEID(s64))
        {
            s64 value = field->GetFieldValue<s64>(instances[0]);
            Text(String::Format("{}", value));
        }
        else if (fieldDeclId == TYPEID(f32))
        {
            f32 value = field->GetFieldValue<f32>(instances[0]);
            Text(String::Format("{}", value));
        }
        else if (fieldDeclId == TYPEID(f64))
        {
            f64 value = field->GetFieldValue<f64>(instances[0]);
            Text(String::Format("{}", value));
        }
    }

    NumericEditorField::Self& NumericEditorField::NumericType(TypeId type)
    {
        if (numericType == type)
            return *this;

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

