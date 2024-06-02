#pragma once

namespace CE
{
    CLASS()
    class FUSIONCORE_API FNullWidget : public FWidget
    {
        CE_CLASS(FNullWidget, FWidget)
    public:

        FNullWidget();

    protected:

        void Construct() override;

        FUSION_TESTS;
    };
    
}

#include "FNullWidget.rtti.h"
