#pragma once

namespace CE
{


    CLASS()
    class FUSIONCORE_API FMenuBarStyle : public FStyle
    {
        CE_CLASS(FMenuBarStyle, FStyle)
    public:

        FMenuBarStyle();

        SubClass<FWidget> GetWidgetClass() const override;

        void MakeStyle(FWidget& widget) override;

        FIELD()
        FBrush background;

        FIELD()
        f32 borderWidth = 0;

        FIELD()
        Color borderColor;

        FIELD()
        Vec4 padding;

        FIELD()
        Vec4 itemPadding;

        FIELD()
        FBrush itemBackground;

        FIELD()
        FBrush itemHoverBackground;

        FIELD()
        Color itemBorderColor;

        FIELD()
        Color itemHoverBorderColor;

        FIELD()
        f32 itemBorderWidth = 0;
    };


    CLASS()
    class FUSIONCORE_API FMenuPopupStyle : public FStyle
    {
        CE_CLASS(FMenuPopupStyle, FStyle)
    public:

        FMenuPopupStyle();

        SubClass<FWidget> GetWidgetClass() const override;

        void MakeStyle(FWidget& widget) override;

        FIELD()
        FBrush background;

        FIELD()
        f32 borderWidth = 0;

        FIELD()
        Color borderColor;

        FIELD()
        Vec4 padding;

        FIELD()
        Vec4 itemPadding;

        FIELD()
        FBrush itemBackground;

        FIELD()
        FBrush itemHoverBackground;

        FIELD()
        Color itemBorderColor;

        FIELD()
        Color itemHoverBorderColor;

        FIELD()
        f32 itemBorderWidth = 0;

    };
    
} // namespace CE

#include "FMenuStyle.rtti.h"
