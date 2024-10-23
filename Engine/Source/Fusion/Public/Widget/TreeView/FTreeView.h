#pragma once

namespace CE
{
    typedef ScriptDelegate<f32(const FModelIndex&)> FRowHeightDelegate;
    typedef ScriptDelegate<FTreeViewRow&(const FModelIndex&)> FGenerateRowDelegate;

    CLASS()
    class FUSION_API FTreeView : public FStyledWidget
    {
        CE_CLASS(FTreeView, FStyledWidget)
    public:

        // - Public API -

        void OnModelUpdate();

        FStyledWidget* GetHeaderContainer() const { return headerContainer; }

    protected:

        FTreeView();

        void Construct() override;

        void OnFusionPropertyModified(const CE::Name& propertyName) override;

        FStyledWidget* headerContainer = nullptr;
        FStyledWidget* containerStyle = nullptr;
        FTreeViewHeader* header = nullptr;
        FTreeViewContainer* container = nullptr;

        bool isVerticalScrollVisible = false;
        bool isVerticalScrollDragged = false;
        bool isVerticalScrollHighlighted = false;

    public: // - Fusion Properties -

        FUSION_PROPERTY(f32, VerticalScrollSensitivity);

        FUSION_PROPERTY(FBrush, ScrollBarBackground);
        FUSION_PROPERTY(FPen, ScrollBarBackgroundPen);
        FUSION_PROPERTY(FBrush, ScrollBarBrush);
        FUSION_PROPERTY(FBrush, ScrollBarHoverBrush);
        FUSION_PROPERTY(FPen, ScrollBarPen);
        FUSION_PROPERTY(FPen, ScrollBarHoverPen);
        FUSION_PROPERTY(FShape, ScrollBarShape);
        FUSION_LAYOUT_PROPERTY(float, ScrollBarWidth);
        FUSION_LAYOUT_PROPERTY(float, ScrollBarMargin);

        FUSION_PROPERTY(FBrush, RowBackground);
        FUSION_PROPERTY(FBrush, RowBackgroundAlternate);

        FUSION_PROPERTY(FTreeViewModel*, Model);

        FUSION_PROPERTY(int, ExpandableColumn);
        FUSION_LAYOUT_PROPERTY(f32, RowHeight);
        FUSION_LAYOUT_PROPERTY(FRowHeightDelegate, RowHeightDelegate);

        FUSION_LAYOUT_PROPERTY(FGenerateRowDelegate, GenerateRowDelegate);

        Self& Header(FTreeViewHeader& header);

        FUSION_WIDGET;
        friend class FTreeViewContainer;
        friend class FTreeViewStyle;
    };
    
}

#include "FTreeView.rtti.h"