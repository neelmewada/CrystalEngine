#pragma once

namespace CE
{
    CLASS()
    class FUSIONCORE_API FComboBoxStyle : public FStyle
    {
        CE_CLASS(FComboBoxStyle, FStyle)
    public:

        FComboBoxStyle();

        SubClass<FWidget> GetWidgetClass() const override;

        void MakeStyle(FWidget& widget) override;

        FIELD()
        FBrush background;

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

    CLASS()
    class FUSIONCORE_API FComboBoxPopupStyle : public FStyle
    {
	    CE_CLASS(FComboBoxPopupStyle, FStyle)
    public:

        FComboBoxPopupStyle();

        SubClass<FWidget> GetWidgetClass() const override;

        void MakeStyle(FWidget& widget) override;

        FIELD()
        FBrush background;

        FIELD()
        Color borderColor;

        FIELD()
        f32 borderWidth = 0;

    };

    CLASS()
    class FUSIONCORE_API FComboBoxItemStyle : public FStyle
    {
	    CE_CLASS(FComboBoxItemStyle, FStyle)
    public:

        FComboBoxItemStyle();

        SubClass<FWidget> GetWidgetClass() const override;

        void MakeStyle(FWidget& widget) override;

        FIELD()
        FShapeType shape = FShapeType::Rect;

        FIELD()
        FShapeType hoverShape = FShapeType::Rect;

        FIELD()
        FShapeType selectedShape = FShapeType::Rect;

        FIELD()
        Vec4 shapeCornerRadius;

        FIELD()
        FBrush background;

        FIELD()
        FBrush hoverBackground;

        FIELD()
        FBrush selectedBackground;

        FIELD()
        Color borderColor;

        FIELD()
        Color hoverBorderColor;

        FIELD()
        Color selectedBorderColor;

        FIELD()
        f32 borderWidth = 0.0f;
    };

    
} // namespace CE

#include "FComboBoxStyle.rtti.h"
