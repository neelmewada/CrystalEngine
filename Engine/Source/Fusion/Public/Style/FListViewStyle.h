#pragma once

namespace CE
{
    CLASS()
    class FUSION_API FListViewStyle : public FStyledWidgetStyle
    {
        CE_CLASS(FListViewStyle, FStyledWidgetStyle)
    public:

        virtual ~FListViewStyle();

        SubClass<FWidget> GetWidgetClass() const override;

        void MakeStyle(FWidget& widget) override;

    protected:

        FListViewStyle();

    public:

        FIELD()
        FBrush itemBackground;

        FIELD()
        FBrush selectedItemBackground;

        FIELD()
        FBrush hoveredItemBackground;

        FIELD()
        Color itemBorderColor;

        FIELD()
        Color selectedItemBorderColor;

        FIELD()
        Color hoveredItemBorderColor;

        FIELD()
        f32 itemBorderWidth = 0;

    };
    
} // namespace CE

#include "FListViewStyle.rtti.h"
