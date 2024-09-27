#pragma once

namespace CE
{
    CLASS()
    class FUSIONCORE_API FStyledWidgetStyle : public FStyle
    {
        CE_CLASS(FStyledWidgetStyle, FStyle)
    public:

        virtual ~FStyledWidgetStyle();

        SubClass<FWidget> GetWidgetClass() const override;

        void MakeStyle(FWidget& widget) override;

    protected:

        FStyledWidgetStyle();

    public:

        FIELD()
        FBrush background;

        FIELD()
        FShape shape = FShapeType::Rect;

        FIELD()
        Color borderColor;

        FIELD()
        f32 borderWidth = 0.0f;

    };
    
} // namespace CE

#include "FStyledWidgetStyle.rtti.h"
