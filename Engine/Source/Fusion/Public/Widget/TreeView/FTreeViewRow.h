#pragma once

namespace CE
{
    CLASS()
    class FUSION_API FTreeViewRow : public FStyledWidget
    {
        CE_CLASS(FTreeViewRow, FStyledWidget)
    public:

        // - Public API -

    protected:

        FTreeViewRow();

        void Construct() override;

    public: // - Fusion Properties - 


        FUSION_WIDGET;
        friend class FTreeView;
    };
    
}

#include "FTreeViewRow.rtti.h"
