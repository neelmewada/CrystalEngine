#pragma once

namespace CE
{

    CLASS()
    class FUSIONCORE_API FWindow : public FCompoundWidget
    {
        CE_CLASS(FWindow, FCompoundWidget)
    public:

        FWindow();

        void Construct() override;

    public:


    private: // - Fields -


        FUSION_TESTS;
        FUSION_WIDGET;
    };
    
}

#include "FWindow.rtti.h"
