#pragma once

namespace CE
{
    CLASS()
    class FUSIONCORE_API FTitleBar : public FStyledWidget
    {
        CE_CLASS(FTitleBar, FStyledWidget)
    public:

        FTitleBar();

    protected:

        void Construct() override final;

    protected: // - Fusion Fields -


    public: // - Fusion Properties - 


        FUSION_WIDGET;
    };
    
}

#include "FTitleBar.rtti.h"
