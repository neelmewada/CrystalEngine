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
    class FUSIONCORE_API FButtonCustomStyle : public FButtonStyle
    {
        CE_CLASS(FButtonCustomStyle, FButtonStyle)
    public:

        FButtonCustomStyle();

        void MakeStyle(FWidget& widget) override;

        FIELD()
        FBrush background{};

        FIELD()
        FBrush hoveredBackground{};

        FIELD()
        FBrush pressedBackground{};

        FIELD()
        Vec4 cornerRadius = Vec4(5, 5, 5, 5);

        FIELD()
        f32 borderWidth = 1.0f;

        FIELD()
        Color borderColor{};

        //! @brief The amount of distance the content of button should move in +Y direction when pressed
        FIELD()
        f32 contentMoveY = 2.5f;
    };
    
} // namespace CE

#include "FButtonStyle.rtti.h"
