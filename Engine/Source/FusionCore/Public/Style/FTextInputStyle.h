#pragma once

namespace CE
{
    CLASS(Abstract)
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
        FBrush background;

        FIELD()
        FBrush editingBackground;

        FIELD()
        FBrush hoverBackground;

        FIELD()
        Vec4 cornerRadius;

        FIELD()
        f32 borderWidth = 1.0f;

        FIELD()
        Color borderColor;

        FIELD()
        Color hoverBorderColor;

        FIELD()
        Color editingBorderColor;
    };
    
} // namespace CE

#include "FTextInputStyle.rtti.h"