#pragma once

namespace CE
{

    CLASS()
    class FUSIONCORE_API FWindow : public FCompoundWidget
    {
        CE_CLASS(FWindow, FCompoundWidget)
    public:

        FWindow();

        bool SupportsMouseEvents() const override { return true; }

    public:


    private: // - Fields -


        FUSION_WIDGET;
    };
    
}

#include "FWindow.rtti.h"
