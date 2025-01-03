#pragma once

namespace CE
{
    CLASS()
    class FUSIONCORE_API FTabViewStyle : public FStyle
    {
        CE_CLASS(FTabViewStyle, FStyle)
    public:

        FTabViewStyle();

        virtual ~FTabViewStyle();

        SubClass<FWidget> GetWidgetClass() const override;

        void MakeStyle(FWidget& widget) override;


        FIELD()
        FBrush tabItemBackground;

        FIELD()
        FBrush tabItemActiveBackground;

        FIELD()
        FBrush tabItemHoverBackground;

        FIELD()
        FShape tabItemShape;

        FIELD()
        FBrush containerBackground;

        FIELD()
        Color containerBorderColor;

        FIELD()
        f32 containerBorderWidth = 0.0f;

    };
    
} // namespace CE

#include "FTabViewStyle.rtti.h"
