#include "EditorSystem.h"

namespace CE::Editor
{

    NumericFieldInput::NumericFieldInput()
    {
        receiveDragEvents = true;
    }

    NumericFieldInput::~NumericFieldInput()
    {
        
    }

    void NumericFieldInput::SetFieldType(NumericFieldType fieldType)
    {
        this->fieldType = fieldType;

        switch (fieldType)
        {
        case NumericFieldType::Float32:
        case NumericFieldType::Float64:
            SetInputValidator(CFloatInputValidator);
            break;
        case NumericFieldType::Uint8:
        case NumericFieldType::Uint16:
        case NumericFieldType::Uint32:
        case NumericFieldType::Uint64:
            SetInputValidator(CUnsignedIntegerInputValidator);
            break;
        case NumericFieldType::Int8:
        case NumericFieldType::Int16:
        case NumericFieldType::Int32:
        case NumericFieldType::Int64:
            SetInputValidator(CIntegerInputValidator);
            break;
        }
    }

    void NumericFieldInput::SetPrecision(u32 precision)
    {
        this->precision = precision;
    }

    void NumericFieldInput::SetRange(f64 min, f64 max)
    {
        useRange = true;
        rangeMin = min;
        rangeMax = max;

        SetNeedsPaint();
    }

    bool NumericFieldInput::PostComputeStyle()
    {
        bool base = Super::PostComputeStyle();

        if (IsEditing())
        {
            hoverCursor = CCursor::Edit;
        }
        else
        {
            hoverCursor = CCursor::SizeH;
        }

        return base;
    }

    void NumericFieldInput::OnValidateText()
    {
	    Super::OnValidateText();

        if (fieldType == NumericFieldType::Float32 || fieldType == NumericFieldType::Float64)
        {
            if (String::TryParse(text, floatValue))
            {
                if (useRange)
                {
                    floatValue = Math::Clamp(floatValue, rangeMin, rangeMax);
                }
                text = String::Format("{:.{}f}", floatValue, precision);
            }
        }
        else if (fieldType == NumericFieldType::Uint8 || fieldType == NumericFieldType::Uint16 ||
            fieldType == NumericFieldType::Uint32 || fieldType == NumericFieldType::Uint64)
        {
            String::TryParse(text, unsignedValue);

            if (useRange)
            {
                unsignedValue = Math::Clamp(unsignedValue, (u64)rangeMin, (u64)rangeMax);
            }

            floatValue = unsignedValue;
        }
        else if (fieldType == NumericFieldType::Int8 || fieldType == NumericFieldType::Int16 ||
            fieldType == NumericFieldType::Int32 || fieldType == NumericFieldType::Int64)
        {
            String::TryParse(text, signedValue);

            if (useRange)
            {
                signedValue = Math::Clamp(signedValue, (s64)rangeMin, (s64)rangeMax);
            }

            floatValue = signedValue;
        }
    }

    void NumericFieldInput::Construct()
    {
        Super::Construct();

        SetSelectAllOnEdit(true);
    }

    void NumericFieldInput::HandleEvent(CEvent* event)
    {
        if (event->IsMouseEvent() && IsEditable())
        {
            CMouseEvent* mouseEvent = static_cast<CMouseEvent*>(event);
            Vec2 mouseDelta = mouseEvent->mousePos - mouseEvent->prevMousePos;
            
            if (mouseEvent->type == CEventType::MousePress && mouseEvent->button == MouseButton::Left && !mouseEvent->isDoubleClick && !IsEditing())
            {
                startMousePos = mouseEvent->mousePos;
                event->Consume(this);
            }
            else if (mouseEvent->type == CEventType::MouseRelease && mouseEvent->button == MouseButton::Left && !IsEditing())
            {
                Vec2 diff = mouseEvent->mousePos - startMousePos;

                if (diff.GetSqrMagnitude() < 2)
                {
                    StartEditing();
	                event->Consume(this);
                }
            }
            else if (mouseEvent->type == CEventType::DragBegin && mouseEvent->button == MouseButton::Left && !IsEditing())
            {
                CDragEvent* dragEvent = static_cast<CDragEvent*>(event);

                dragEvent->draggedWidget = this;
                dragEvent->Consume(this);
            }
            else if (mouseEvent->type == CEventType::DragMove && mouseEvent->button == MouseButton::Left && !IsEditing())
            {
                f32 deltaX = mouseDelta.x;
                CDragEvent* dragEvent = static_cast<CDragEvent*>(event);

                float sensitivity = (IsSignedInt() || IsUnsignedInt()) ? integerSensitivity : floatSensitivity;

                floatValue += deltaX * sensitivity;

                if (useRange)
                {
                    floatValue = Math::Clamp(floatValue, rangeMin, rangeMax);
                }

                if (IsSignedInt())
                {
                    signedValue = (s64)floatValue;
                    SetTextInternal(String::Format("{}", signedValue));
                }
                else if (IsUnsignedInt())
                {
                    unsignedValue = (u64)floatValue;
                    SetTextInternal(String::Format("{}", unsignedValue));
                }
                else
                {
                    SetTextInternal(String::Format("{:.{}f}", floatValue, precision));
                }

                dragEvent->draggedWidget = this;
                dragEvent->Consume(this);
            }
        }

	    Super::HandleEvent(event);
    }

    void NumericFieldInput::OnPaintEarly(CPaintEvent* paintEvent)
    {
	    Super::OnPaintEarly(paintEvent);

        CPainter* painter = paintEvent->painter;

        Vec2 pos = GetComputedLayoutTopLeft();
        Vec2 size = GetComputedLayoutSize();
        Vec4 borderRadius = computedStyle.GetBorderRadius();
        Color borderColor = computedStyle.properties[CStylePropertyType::BorderColor].color;

        if (useRange)
        {
            f32 padding = 2.5f;
            Rect fullRect = Rect::FromSize(pos + Vec2(1, 1) * padding, size - Vec2(1, 1) * padding * 2);
            f64 ratio = Math::Clamp01((floatValue - rangeMin) / (rangeMax - rangeMin));
            fullRect = Rect::FromSize(fullRect.min, fullRect.GetSize() * Vec2(ratio, 1));

            CPen pen{};
            CBrush brush = CBrush(borderColor);

            painter->SetPen(pen);
            painter->SetBrush(brush);

            painter->DrawRoundedRect(fullRect, borderRadius * 0.5f);
        }
    }

}

