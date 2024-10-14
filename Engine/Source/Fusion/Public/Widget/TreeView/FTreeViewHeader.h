#pragma once

namespace CE
{
    CLASS()
    class FUSION_API FTreeViewHeader : public FWidget
    {
        CE_CLASS(FTreeViewHeader, FWidget)
    public:

        // - Public API -

    protected:

        FTreeViewHeader();

        void Construct() override;

    public: // - Fusion Properties - 


        FUSION_WIDGET;
    };
    
}

#include "FTreeViewHeader.rtti.h"
