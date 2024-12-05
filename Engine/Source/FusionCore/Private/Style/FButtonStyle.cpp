#include "FusionCore.h"

namespace CE
{

    FButtonStyle::FButtonStyle()
    {

    }

    SubClass<FWidget> FButtonStyle::GetWidgetClass() const
    {
        return FButton::StaticType();
    }

    FCustomButtonStyle::FCustomButtonStyle()
    {
        contentMoveY = 1.0f;
    }

    void FCustomButtonStyle::MakeStyle(FWidget& widget)
    {
        FButton& button = widget.As<FButton>();

        FBrush bg = background;
        Color border = borderColor;

        if (button.IsInteractionDisabled())
        {
            bg = disabledBackground;
        }
        else if (button.IsPressed() && button.IsHovered())
        {
	        bg = pressedBackground;
            border = pressedBorderColor;
        }
        else if (button.IsHovered())
        {
	        bg = hoveredBackground;
            border = hoveredBorderColor;
        }

        FShape shape = backgroundShape;
        if (backgroundShape == FShapeType::RoundedRect)
        {
            shape.SetCornerRadius(cornerRadius);
        }

        button
            .Background(bg)
            .BackgroundShape(shape)
            .ClipShape(FShapeType::None)
            .BorderWidth(borderWidth)
            .BorderColor(border)
            ;

        if (button.GetChild() == nullptr || abs(contentMoveY) < 0.001f)
            return;

        FWidget& child = *button.GetChild();
        f32 childHeight = child.GetComputedSize().y;

        child
            .Translation(Vec2(0, button.IsPressed() && button.IsHovered() ? contentMoveY : 0))
            ;
    }

} // namespace CE

