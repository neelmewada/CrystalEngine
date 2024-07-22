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

    FButtonCustomStyle::FButtonCustomStyle()
    {

    }

    void FButtonCustomStyle::MakeStyle(FWidget& widget)
    {
        FButton& button = widget.As<FButton>();

        FBrush bg = background;

        if (button.IsPressed() && button.IsHovered())
            bg = pressedBackground;
        else if (button.IsHovered())
            bg = hoveredBackground;

        button
            .Background(bg)
            .BackgroundShape(FRoundedRectangle(cornerRadius))
            .ClipShape(FShapeType::None)
            .BorderWidth(borderWidth)
            .BorderColor(borderColor)
            ;

        if (button.GetChild() == nullptr || abs(contentMoveY) < 0.001f)
            return;

        FWidget& child = *button.GetChild();
        f32 childHeight = child.GetComputedSize().y;

        child
            .Translation(Vec2(0, button.IsPressed() && button.IsHovered() ? childHeight * 0.1f : 0))
            ;
    }

} // namespace CE

