#pragma once

namespace CE
{
    CLASS()
    class FUSION_API FListViewContainer : public FContainerWidget
    {
        CE_CLASS(FListViewContainer, FContainerWidget)
    public:

        FListViewContainer();

        virtual ~FListViewContainer();

        // - Public API -

        void CalculateIntrinsicSize() override;

        void PlaceSubWidgets() override;

    protected:

        void OnPaint(FPainter* painter) override;

    public: // - Fusion Properties - 

        FUSION_LAYOUT_PROPERTY(f32, Gap);

        FUSION_WIDGET;
    };
    
}

#include "FListViewContainer.rtti.h"
