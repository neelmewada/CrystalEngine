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

        FBrush background = this->background;

        Color border = borderColor;
        if (input.IsEditing())
        {
	        border = editingBorderColor;
            background = editingBackground;
        }
        else if (input.IsHovered())
        {
	        border = hoverBorderColor;
            background = hoverBackground;
        }
        
        input
            .Background(background)
            .BackgroundShape(FRoundedRectangle(cornerRadius))
            .Border(border, borderWidth)
            ;
    }

} // namespace CE
