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
        Vec4 tabItemPadding;

        FIELD()
        FShape tabItemShape;

    };
    
} // namespace CE

#include "FTabViewStyle.rtti.h"
