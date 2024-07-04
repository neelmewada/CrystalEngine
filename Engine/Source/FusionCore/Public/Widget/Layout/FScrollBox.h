#pragma once

namespace CE
{
    CLASS()
    class FUSIONCORE_API FScrollBox : public FCompoundWidget
    {
        CE_CLASS(FScrollBox, FCompoundWidget)
    public:

        FScrollBox();

        void CalculateIntrinsicSize() override;

        void PlaceSubWidgets() override;

    protected:

        FWidget* HitTest(Vec2 mousePosition) override;

        void OnPaint(FPainter* painter) override;

    public: // - Fusion Properties - 

        FUSION_LAYOUT_PROPERTY(bool, VerticalScroll);
        FUSION_LAYOUT_PROPERTY(bool, HorizontalScroll);

        FUSION_PROPERTY(FBrush, Background);
        FUSION_PROPERTY(Color, BorderColor);
        FUSION_PROPERTY(f32, BorderWidth);

        FUSION_PROPERTY(FBrush, ScrollBarBackground);
        FUSION_PROPERTY(FPen, ScrollBarBackgroundPen);
        FUSION_PROPERTY(FBrush, ScrollBarBrush);
        FUSION_PROPERTY(FBrush, ScrollBarHoverBrush);
        FUSION_PROPERTY(FPen, ScrollBarPen);
        FUSION_PROPERTY(FPen, ScrollBarHoverPen);
        FUSION_PROPERTY(FShape, ScrollBarShape);
        FUSION_LAYOUT_PROPERTY(float, ScrollBarWidth);
        FUSION_LAYOUT_PROPERTY(float, ScrollBarMargin);

    private:

        bool isVerticalScrollVisible = false;
        bool isHorizontalScrollVisible = false;

        FUSION_WIDGET;
    };
    
}

#include "FScrollBox.rtti.h"
