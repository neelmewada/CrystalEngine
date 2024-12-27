#include "EditorCore.h"

namespace CE::Editor
{

    ColorPickerPreview::ColorPickerPreview()
    {

    }

    void ColorPickerPreview::CalculateIntrinsicSize()
    {
        intrinsicSize = Vec2(64, 32);

        ApplyIntrinsicSizeConstraints();
    }

    void ColorPickerPreview::OnPaint(FPainter* painter)
    {
	    Super::OnPaint(painter);

        static CE::Name gridPattern = "/Engine/Resources/Icons/TransparentPattern";

        Vec2 size = GetComputedSize();

        painter->SetBrush(m_PreviewColor.WithAlpha(1.0f));
        painter->SetPen(FPen());

        painter->DrawRect(Rect::FromSize(Vec2(), Vec2(size.x / 2, size.y)));

        if (m_PreviewColor.a < 0.9999f)
        {
            FBrush gridBrush = FBrush(gridPattern);
            gridBrush.SetBrushTiling(FBrushTiling::TileXY);
            gridBrush.SetBrushSize(Vec2(16, 16));

            painter->SetBrush(gridBrush);

            painter->DrawRect(Rect::FromSize(Vec2(size.x / 2, 0), Vec2(size.x / 2, size.y)));
        }

        painter->SetBrush(m_PreviewColor);

        painter->DrawRect(Rect::FromSize(Vec2(size.x / 2, 0), Vec2(size.x / 2, size.y)));
    }

    void ColorPickerPreview::Construct()
    {
        Super::Construct();
        
    }

}

