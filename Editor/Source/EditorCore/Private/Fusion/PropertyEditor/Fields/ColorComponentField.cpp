#include "EditorCore.h"

namespace CE::Editor
{
    static constexpr f32 GradientHeight = 4;
    static constexpr f32 InputPadding = 3;

    ColorComponentField::ColorComponentField()
    {
        isRanged = true;
        rangeSliderPadding = Vec4(InputPadding, InputPadding, InputPadding, InputPadding + 0.5f + GradientHeight + InputPadding);
        min = 0;
        max = 1;
    }

    void ColorComponentField::Construct()
    {
        Super::Construct();

        input->Padding(Vec4(11, 3, 8, InputPadding + GradientHeight + InputPadding));
    }

    void ColorComponentField::OnPaintBeforeText(FPainter* painter)
    {
	    Super::OnPaintBeforeText(painter);

        Vec2 gradientPos = Vec2(rangeSliderPadding.left, GetComputedSize().height - rangeSliderPadding.top - GradientHeight);
        Vec2 gradientSize = Vec2(GetComputedSize().width - rangeSliderPadding.left - rangeSliderPadding.right, GradientHeight);

        static CE::Name gridPattern = "/Engine/Resources/Icons/TransparentPattern";

        if (m_Transparency)
        {
            FBrush gridBrush = FBrush(gridPattern);
            gridBrush.SetBrushTiling(FBrushTiling::TileXY);
            gridBrush.SetBrushSize(Vec2(GradientHeight, GradientHeight));

            painter->SetBrush(gridBrush);
            painter->SetPen(FPen());

            painter->DrawRoundedRect(Rect::FromSize(gradientPos, gradientSize), Vec4(1, 1, 1, 1) * GradientHeight / 2);
        }

        if (m_Gradient.IsValid())
        {
            painter->SetBrush(m_Gradient);
            painter->SetPen(FPen());

            painter->DrawRoundedRect(Rect::FromSize(gradientPos, gradientSize), Vec4(1, 1, 1, 1) * GradientHeight / 2);
        }
    }

}

