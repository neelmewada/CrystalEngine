#pragma once

namespace CE
{
    CLASS(Abstract)
    class FUSIONCORE_API FButtonStyle : public FStyle
    {
        CE_CLASS(FButtonStyle, FStyle)
    public:

        FButtonStyle();

        SubClass<FWidget> GetWidgetClass() const override;

    };

    CLASS()
    class FUSIONCORE_API FPlainButtonStyle : public FButtonStyle
    {
        CE_CLASS(FPlainButtonStyle, FButtonStyle)
    public:

        FPlainButtonStyle();

        void MakeStyle(FWidget& widget) override;

        FIELD()
        Vec4 buttonPadding = Vec4(10, 5, 10, 5);

        FIELD()
        Color background{};

        FIELD()
        Color hoveredBackground{};

        FIELD()
        Color pressedBackground{};

        FIELD()
        Vec4 cornerRadius = Vec4(5, 5, 5, 5);

        FIELD()
        f32 borderWidth = 1.0f;

        FIELD()
        Color borderColor{};

    };
    
} // namespace CE

#include "FButtonStyle.rtti.h"
