#pragma once

namespace CE
{
    CLASS()
    class FUSIONCORE_API FMenuItem : public FStyledWidget
    {
        CE_CLASS(FMenuItem, FStyledWidget)
    public:

        FMenuItem();

    protected:


    public: // - Fusion Properties - 


        FUSION_WIDGET;
    };
    
}

#include "FMenuItem.rtti.h"
