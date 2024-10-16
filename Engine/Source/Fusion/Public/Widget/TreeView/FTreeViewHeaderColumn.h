#pragma once

namespace CE
{
    CLASS()
    class FUSION_API FTreeViewHeaderColumn : public FStyledWidget
    {
        CE_CLASS(FTreeViewHeaderColumn, FStyledWidget)
    public:

        // - Public API -

    protected:

        FTreeViewHeaderColumn();

        void Construct() override;

        FLabel* label = nullptr;

    public: // - Fusion Properties - 

        FUSION_PROPERTY_WRAPPER2(Text, label, Title);

        FUSION_WIDGET;
    };
    
}

#include "FTreeViewHeaderColumn.rtti.h"
