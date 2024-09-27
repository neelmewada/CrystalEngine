#pragma once

namespace CE
{
    CLASS()
    class FUSION_API FListItemStyle : public FStyle
    {
        CE_CLASS(FListItemStyle, FStyle)
    public:

        virtual ~FListItemStyle();

        SubClass<FWidget> GetWidgetClass() const override;

        void MakeStyle(FWidget& widget) override;

    protected:

        FListItemStyle();

    public:

    };
    
} // namespace CE

#include "FListItemStyle.rtti.h"
