#pragma once

namespace CE
{
    CLASS()
    class FUSIONCORE_API FScrollBox : public FStyledWidget
    {
        CE_CLASS(FScrollBox, FStyledWidget)
    public:

        FScrollBox();

    public: // - Fusion Properties - 


        FUSION_WIDGET;
    };
    
}

#include "FScrollBox.rtti.h"
