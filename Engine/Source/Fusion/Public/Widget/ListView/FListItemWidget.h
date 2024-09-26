#pragma once

namespace CE
{
    CLASS()
    class FUSION_API FListItemWidget : public FStyledWidget
    {
        CE_CLASS(FListItemWidget, FStyledWidget)
    public:

        FListItemWidget();

    protected:

    public: // - Fusion Properties - 


        FUSION_WIDGET;
    };
    
}

#include "FListItemWidget.rtti.h"
