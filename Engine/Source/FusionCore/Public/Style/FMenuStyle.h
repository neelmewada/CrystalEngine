#pragma once

namespace CE
{
    CLASS()
    class FUSIONCORE_API FMenuStyle : public FStyle
    {
        CE_CLASS(FMenuStyle, FStyle)
    public:

        FMenuStyle();

        SubClass<FWidget> GetWidgetClass() const override;

        void MakeStyle(FWidget& widget) override;

    };
    
} // namespace CE

#include "FMenuStyle.rtti.h"
