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

        void PrecomputeIntrinsicSize() override;

        void PlaceSubWidgets() override;

        void OnPaint(FPainter* painter) override;

    protected:

        void Construct() override;

    protected:  // - Fusion Fields -

        FIELD()
        CE::HAlign m_ContentHAlign = HAlign::Fill;

        FIELD()
        CE::VAlign m_ContentVAlign = VAlign::Fill;

    public:  // - Fusion Properties -

        FUSION_LAYOUT_PROPERTY(ContentHAlign);

        FUSION_LAYOUT_PROPERTY(ContentVAlign);


        FUSION_TESTS;
        FUSION_WIDGET;
    };
    
}

#include "FOverlayStack.rtti.h"
