#pragma once

namespace CE
{
    CLASS()
    class FUSIONCORE_API FOverlayStack : public FContainerWidget
    {
        CE_CLASS(FOverlayStack, FContainerWidget)
    public:

        FOverlayStack();

        // - Public API -

        void CalculateIntrinsicSize() override;

        void PlaceSubWidgets() override;

        void OnPaint(FPainter* painter) override;

    protected:

        void Construct() override;

    public:  // - Fusion Properties -

        FUSION_LAYOUT_PROPERTY(CE::HAlign, ContentHAlign);

        FUSION_LAYOUT_PROPERTY(CE::VAlign, ContentVAlign);


        FUSION_TESTS;
        FUSION_WIDGET;
    };
    
}

#include "FOverlayStack.rtti.h"
