#include "EditorSystem.h"

namespace CE::Editor
{

    ColorPickerPreview::ColorPickerPreview()
    {
	    
    }

    Vec2 ColorPickerPreview::CalculateIntrinsicSize(f32 width, f32 height)
    {
        return Vec2(32, 16);
    }

    void ColorPickerPreview::SetColor(const Color& color)
    {
        this->color = color;

        SetNeedsPaint();
    }

    void ColorPickerPreview::Construct()
    {
        Super::Construct();

        
    }

    void ColorPickerPreview::OnPaint(CPaintEvent* paintEvent)
    {
	    Super::OnPaint(paintEvent);

        CPainter* painter = paintEvent->painter;

        Vec2 pos = GetComputedLayoutTopLeft();
        Vec2 size = GetComputedLayoutSize();

        static const Name gridPattern = "/Editor/Assets/Images/GridPattern";

        RPI::Texture* image = CApplication::Get()->LoadImage(gridPattern);

        CPen pen = CPen();
        CBrush brush = CBrush(Color::White());
        painter->SetPen(pen);
        painter->SetBrush(brush);

        if (image && color.a < 0.99f)
        {
            Vec2 imageSize = Vec2(image->GetWidth(), image->GetHeight());
            Vec2 scaling = Vec2(1, 1);
            scaling.x = size.width * 0.5f / size.height;
            scaling.x *= 2; scaling.y *= 2;

            painter->DrawTexture(Rect::FromSize(pos + Vec2(size.width * 0.5f, 0), Vec2(size.width * 0.5f, size.height)),
                image, CBackgroundRepeat::Repeat, scaling);
        }

        pen = CPen();

        brush.SetColor(Color(color.r, color.g, color.b, 1.0f));
        painter->SetPen(pen);
        painter->SetBrush(brush);

        painter->DrawRect(Rect::FromSize(pos, Vec2(size.width * 0.5f + 1, size.height)));

        brush.SetColor(Color(color.r, color.g, color.b, color.a));
        painter->SetPen(pen);
        painter->SetBrush(brush);

        painter->DrawRect(Rect::FromSize(pos + Vec2(size.width * 0.5f, 0.0f), Vec2(size.width * 0.5f, size.height)));
    }

} // namespace CE::Editor
