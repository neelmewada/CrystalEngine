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

        

    public: // - Fusion Properties - 


        FUSION_WIDGET;
    };
    
}

#include "FListViewContainer.rtti.h"
