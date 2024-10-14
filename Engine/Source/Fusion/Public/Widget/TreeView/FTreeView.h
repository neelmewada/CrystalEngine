#pragma once

namespace CE
{
    CLASS()
    class FUSION_API FTreeView : public FStyledWidget
    {
        CE_CLASS(FTreeView, FStyledWidget)
    public:

        // - Public API -

    protected:

        FTreeView();

        void Construct() override;

        void OnFusionPropertyModified(const CE::Name& propertyName) override;

        FStyledWidget* headerContainer = nullptr;
        FTreeViewHeader* header = nullptr;
        FTreeViewContainer* container = nullptr;

    public: // - Fusion Properties -

        FUSION_PROPERTY(FBrush, Background);
        FUSION_PROPERTY(FBrush, RowBackground);
        FUSION_PROPERTY(FBrush, RowBackgroundAlternate);

        FUSION_PROPERTY(FTreeViewModel*, Model);
        

        FUSION_WIDGET;
    };
    
}

#include "FTreeView.rtti.h"
