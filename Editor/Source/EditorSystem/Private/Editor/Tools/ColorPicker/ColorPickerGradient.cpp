#include "EditorSystem.h"

namespace CE::Editor
{

    ColorPickerGradient::ColorPickerGradient()
    {

    }

    ColorPickerGradient::~ColorPickerGradient()
    {
        
    }

    void ColorPickerGradient::SetGradient(const CGradient& gradient)
    {
        this->gradient = gradient;

        SetNeedsPaint();
    }

    void ColorPickerGradient::Construct()
    {
        Super::Construct();


    }

    void ColorPickerGradient::OnPaint(CPaintEvent* paintEvent)
    {
	    Super::OnPaint(paintEvent);

        CPainter* painter = paintEvent->painter;

        Vec2 pos = GetComputedLayoutTopLeft();
        Vec2 size = GetComputedLayoutSize();

        Rect rect = Rect::FromSize(pos, size);

        Vec4 borderRadius = computedStyle.GetBorderRadius();

        if (gradient.keys.NonEmpty())
        {
            CPen pen = CPen();
            CBrush brush = CBrush();
            brush.SetGradient(gradient);

            painter->SetPen(pen);
            painter->SetBrush(brush);

            if (borderRadius == Vec4())
            {
                painter->DrawRect(rect);
            }
            else
            {
                painter->DrawRoundedRect(rect, borderRadius);
            }
        }
    }

}

