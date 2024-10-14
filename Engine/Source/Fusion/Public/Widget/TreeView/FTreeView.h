#pragma once

namespace CE
{
    typedef ScriptDelegate<f32(int)> FRowHeightDelegate;
    typedef ScriptDelegate<FTreeViewRow&(int)> FGenerateRowDelegate;

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

        FUSION_LAYOUT_PROPERTY(f32, RowHeight);
        FUSION_LAYOUT_PROPERTY(FRowHeightDelegate, RowHeightDelegate);

        FUSION_LAYOUT_PROPERTY(FGenerateRowDelegate, GenerateRowDelegate);

        FUSION_WIDGET;
    };
    
}

#include "FTreeView.rtti.h"
