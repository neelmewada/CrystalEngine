#pragma once

namespace CE
{

    CLASS()
    class FUSIONCORE_API FWindow : public FWidget
    {
        CE_CLASS(FWindow, FWidget)
    public:

        FWindow();

        void Construct() override;

    public:


    private: // - Fields -


        FUSION_TESTS;
    };
    
}

#include "FWindow.rtti.h"
