#pragma once

namespace CE
{
    CLASS()
    class FUSIONCORE_API FWindow : public FWidget
    {
        CE_CLASS(FWindow, FWidget)
    public:

        FWindow();

    protected:

        void Construct() override;


        FUSION_TESTS;
    };
    
}

#include "FWindow.rtti.h"
