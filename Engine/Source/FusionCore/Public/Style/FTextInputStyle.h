#pragma once

namespace CE
{
    CLASS()
    class FUSIONCORE_API FTextInputStyle : public FStyle
    {
        CE_CLASS(FTextInputStyle, FStyle)
    public:

        FTextInputStyle();

        SubClass<FWidget> GetWidgetClass() const override final;

    protected:

    };

    CLASS()
        class FUSIONCORE_API FTextInputPlainStyle : public FTextInputStyle
    {
        CE_CLASS(FTextInputPlainStyle, FTextInputStyle)
    public:

        FTextInputPlainStyle();

        void MakeStyle(FWidget& widget) override;

        FIELD()
        Color background;

        FIELD()
        Vec4 cornerRadius;

        FIELD()
        f32 borderWidth = 1.0f;

        FIELD()
        Color borderColor;

        FIELD()
        Color borderColorHover;

        FIELD()
        Color borderColorActive;
    };
    
} // namespace CE

#include "FTextInputStyle.rtti.h"