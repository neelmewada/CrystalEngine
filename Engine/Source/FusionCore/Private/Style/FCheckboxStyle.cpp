#include "FusionCore.h"

namespace CE
{

    FCheckboxStyle::FCheckboxStyle()
    {
        contentMoveY = 0;
    }

    SubClass<FWidget> FCheckboxStyle::GetWidgetClass() const
    {
        return FCheckbox::StaticType();
    }

    void FCheckboxStyle::MakeStyle(FWidget& widget)
    {
        Super::MakeStyle(widget);

        FCheckbox& checkbox = widget.As<FCheckbox>();
        FImage& checkmark = *checkbox.GetCheckmark();

        Color tint = foregroundColor;
        if (checkbox.IsInteractionDisabled())
            tint = foregroundDisabledColor;
        else if (checkbox.IsPressed())
            tint = foregroundPressedColor;
        else if (checkbox.IsHovered())
            tint = foregroundHoverColor;

        FBrush bg = checkmark.Background();
        if (bg.GetBrushStyle() == FBrushStyle::Texture)
        {
            bg = bg.WithTint(tint);
            checkmark.Background(bg);
        }
    }

    
}

