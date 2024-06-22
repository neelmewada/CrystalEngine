#include "FusionCore.h"

namespace CE
{
    FTextInputStyle::FTextInputStyle()
    {
    }

    SubClass<FWidget> FTextInputStyle::GetWidgetClass() const
    {
        return FTextInput::Type();
    }

    FTextInputPlainStyle::FTextInputPlainStyle()
    {
    }

    void FTextInputPlainStyle::MakeStyle(FWidget& widget)
    {
        FTextInput& input = (FTextInput&)widget;

        Color border = borderColor;
        if (input.IsEditing())
            border = editingBorderColor;
        else if (input.IsHovered())
            border = hoverBorderColor;

        input
            .Background(FBrush(background))
            .BackgroundShape(FRoundedRectangle(cornerRadius))
            .Border(border, borderWidth)
            ;
    }

} // namespace CE
