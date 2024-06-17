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
			.BackgroundShape(FRectangle())
			.BorderWidth(borderWidth)
			.BorderColor(borderColor)
			//.CornerRadius(cornerRadius)
			.Opacity(1.0f)
            //.Padding(buttonPadding)
            ;
        
        if (button.GetChild() == nullptr)
            return;

        FWidget& child = *button.GetChild();

        child
            .Translation(Vec2(0, button.IsPressed() && button.IsHovered() ? 5 : 0))
            ;
    }

} // namespace CE

