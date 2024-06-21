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

        input
            .Background(FBrush(background))
            .BackgroundShape(FRoundedRectangle(cornerRadius))
            .Border(borderColor, borderWidth)
            ;
    }

} // namespace CE
