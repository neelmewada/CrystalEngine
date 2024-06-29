#pragma once

namespace CE
{
    CLASS(Abstract)
    class FUSIONCORE_API FComboBoxStyle : public FStyle
    {
        CE_CLASS(FComboBoxStyle, FStyle)
    public:

        FComboBoxStyle();

        SubClass<FWidget> GetWidgetClass() const override;

    };

    CLASS()
    class FUSIONCORE_API FComboBoxPlainStyle : public FComboBoxStyle
    {
        CE_CLASS(FComboBoxPlainStyle, FComboBoxStyle)
    public:

        FComboBoxPlainStyle();

        void MakeStyle(FWidget& widget) override;

        FIELD()
        Color background;

        FIELD()
        Color borderColor;

        FIELD()
        Color hoverBorderColor;

        FIELD()
        Color pressedBorderColor;

        FIELD()
        Vec4 cornerRadius = Vec4(5, 5, 5, 5);

        FIELD()
        f32 borderWidth = 1.0f;

        //! @brief The amount of distance the content of button should move in +Y direction when pressed
        FIELD()
        f32 contentMoveY = 2.5f;
    };
    
} // namespace CE

#include "FComboBoxStyle.rtti.h"
