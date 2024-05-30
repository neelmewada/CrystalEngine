#include "EditorSystem.h"

namespace CE::Editor
{

    ColorComponentInput::ColorComponentInput()
    {
        rangePadding = Vec4(4, 4, 4, 10.5f);
    }

    void ColorComponentInput::ShowTransparency(bool show)
    {
        if (show == showTransparency)
            return;

        showTransparency = show;

        SetNeedsPaint();
    }

    void ColorComponentInput::Construct()
    {
        Super::Construct();

        SetRange(0, 1);

    }

    void ColorComponentInput::OnPaint(CPaintEvent* paintEvent)
    {
	    Super::OnPaint(paintEvent);

        CPainter* painter = paintEvent->painter;

        Vec2 pos = GetComputedLayoutTopLeft();
        Vec2 size = GetComputedLayoutSize();
        Vec4 padding = computedStyle.GetPadding();

        if (gradient.keys.GetSize() >= 2)
        {
            CPen pen = CPen();
            painter->SetPen(pen);

            CBrush brush = CBrush();

            constexpr f32 gradientHeight = 6.0f;
            constexpr f32 spacingY = 1.5f;

            Rect gradientRect = Rect::FromSize(pos + Vec2(rangePadding.left, 0), 
                Vec2(size.x, gradientHeight) - Vec2(rangePadding.left + rangePadding.right, 0));
            gradientRect = gradientRect.Translate(Vec2(0, size.height - gradientHeight - rangePadding.top + spacingY));

            static const Name gridPath = "/Editor/Assets/Images/GridSmall";

            Vec4 borderRadius = Vec4(1, 1, 1, 1) * gradientHeight * 0.5f;

            if (showTransparency)
            {
                brush.SetColor(Color::White());
                painter->SetBrush(brush);

                painter->PushClipRect(gradientRect, borderRadius);

                painter->DrawTexture(gradientRect, gridPath, CBackgroundRepeat::RepeatX,
                    Vec2(gradientRect.GetSize().width / gradientRect.GetSize().height, 1),
                    Vec2(0, 0));

                painter->PopClipRect();
            }

            brush.SetGradient(gradient);
            painter->SetBrush(brush);

            painter->DrawRoundedRect(gradientRect, borderRadius);
        }
    }

}

