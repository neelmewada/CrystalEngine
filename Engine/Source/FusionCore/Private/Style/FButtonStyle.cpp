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

    FPlainButtonStyle::FPlainButtonStyle()
    {

    }

    void FPlainButtonStyle::MakeStyle(FWidget& widget)
    {
        FButton& button = widget.As<FButton>();

        Color bgColor = background;

        if (button.IsPressed() && button.IsHovered())
            bgColor = pressedBackground;
        else if (button.IsHovered())
            bgColor = hoveredBackground;

        button
            .Background(FBrush(bgColor))
			.BackgroundShape(FRoundedRectangle(cornerRadius))
			.BorderWidth(borderWidth)
			.BorderColor(borderColor)
			.Opacity(1.0f)
            ;
        
        if (button.GetChild() == nullptr)
            return;

        FWidget& child = *button.GetChild();
        f32 childHeight = child.GetComputedSize().y;

        child
            .Translation(Vec2(0, button.IsPressed() && button.IsHovered() ? childHeight * 0.1f : 0))
            ;
    }

} // namespace CE

