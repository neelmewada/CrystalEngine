#include "FusionCore.h"

namespace CE
{

    FImageButtonStyle::FImageButtonStyle()
    {
        
    }

    SubClass<FWidget> FImageButtonStyle::GetWidgetClass() const
    {
        return FImageButton::StaticType();
    }

    void FImageButtonStyle::MakeStyle(FWidget& widget)
    {
        FImageButton& button = widget.As<FImageButton>();

        FBrush bg = button.Image();

        if (bg.GetBrushStyle() == FBrushStyle::Image)
        {
            bg = bg.WithTint(button.GetButtonState() == FButtonState::Pressed 
                ? pressedTintColor
                : (button.GetButtonState() == FButtonState::Hovered 
                    ? hoverTintColor
                    : tintColor));

            button.Image(bg);
        }

    }

}

