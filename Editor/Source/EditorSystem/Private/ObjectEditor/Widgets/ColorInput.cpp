#include "EditorSystem.h"

namespace CE::Editor
{

    ColorInput::ColorInput()
    {
        clipChildren = true;
    }

    ColorInput::~ColorInput()
    {
	    
    }

    void ColorInput::SetValue(const Color& value)
    {
        if (this->value == value)
            return;

        this->value = value;

        SetNeedsPaint();
    }

    void ColorInput::SetHasAlpha(bool set)
    {
        if (hasAlpha == set)
            return;

        hasAlpha = set;

        if (hasAlpha)
        {
            AddStyleClass("Transparent");
        }
        else
        {
            RemoveStyleClass("Transparent");
        }

        SetNeedsPaint();
    }

    void ColorInput::Construct()
    {
        Super::Construct();

        if (hasAlpha)
        {
            AddStyleClass("Transparent");
        }
        else
        {
            RemoveStyleClass("Transparent");
        }
    }

    void ColorInput::OnPaintEarly(CPaintEvent* paintEvent)
    {
	    Super::OnPaintEarly(paintEvent);

        CPainter* painter = paintEvent->painter;

        Vec2 pos = GetComputedLayoutTopLeft();
        Vec2 size = GetComputedLayoutSize();

        Color opaque = value;
        opaque.a = 1.0f;

        if (hasAlpha)
        {
            CPen pen = CPen();
            CBrush brush = CBrush(opaque);

            painter->SetPen(pen);
            painter->SetBrush(brush);

            painter->DrawRect(Rect::FromSize(pos + Vec2(size.x * 0.5f, 0), size * Vec2(0.5f, 1)));

            brush.SetColor(value);
            painter->SetBrush(brush);

            painter->DrawRect(Rect::FromSize(pos, size * Vec2(0.5f, 1)));
        }
        else
        {
            CPen pen = CPen();
            CBrush brush = CBrush(opaque);

            painter->SetPen(pen);
            painter->SetBrush(brush);

            painter->DrawRect(Rect::FromSize(pos, size));
        }
    }

} // namespace CE::Editor
