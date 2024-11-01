#pragma once

namespace CE
{
    CLASS()
    class FUSIONCORE_API FCheckboxStyle : public FCustomButtonStyle
    {
        CE_CLASS(FCheckboxStyle, FCustomButtonStyle)
    protected:

        FCheckboxStyle();

    public: // - Public API -

        SubClass<FWidget> GetWidgetClass() const override;

        void MakeStyle(FWidget& widget) override;

        FIELD()
        Color foregroundDisabledColor = Color::White();

        FIELD()
        Color foregroundColor = Color::White();

        FIELD()
        Color foregroundHoverColor = Color::White();

        FIELD()
        Color foregroundPressedColor = Color::White();
    };
    
}

#include "FCheckboxStyle.rtti.h"
