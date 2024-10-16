#pragma once

namespace CE
{
    CLASS()
    class FUSION_API FTreeViewStyle : public FStyle
    {
        CE_CLASS(FTreeViewStyle, FStyle)
    public:

        virtual ~FTreeViewStyle();

    protected:

        FTreeViewStyle();

        SubClass<FWidget> GetWidgetClass() const override;

        void MakeStyle(FWidget& widget) override;

    public:

        FIELD()
        FBrush headerBackground = Color::RGBA(47, 47, 47);

        FIELD()
        Color headerSeparator = Color::RGBA(26, 26, 26);

        FIELD()
        f32 separatorWidth = 2.5f;

        FIELD()
        FBrush background = Color::RGBA(26, 26, 26);

    };
    
} // namespace CE

#include "FTreeViewStyle.rtti.h"
