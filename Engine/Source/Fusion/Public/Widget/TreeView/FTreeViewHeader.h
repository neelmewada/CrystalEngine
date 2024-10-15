#pragma once

namespace CE
{
    CLASS()
    class FUSION_API FTreeViewHeader : public FCompoundWidget
    {
        CE_CLASS(FTreeViewHeader, FCompoundWidget)
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
