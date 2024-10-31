#pragma once

namespace CE
{
    CLASS()
    class FUSIONCORE_API FCheckbox : public FStyledWidget
    {
        CE_CLASS(FCheckbox, FStyledWidget)
    protected:

        FCheckbox();

        void Construct() override;

    public: // - Public API -

        bool SupportsMouseEvents() const override { return true; }

    public: // - Fusion Properties - 


        FUSION_WIDGET;
    };
    
}

#include "FCheckbox.rtti.h"
