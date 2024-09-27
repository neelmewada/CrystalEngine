#pragma once

namespace CE
{

    CLASS()
    class FUSION_API FListItemWidget : public FStyledWidget
    {
        CE_CLASS(FListItemWidget, FStyledWidget)
    public:

        FListItemWidget();

    protected:

        void HandleEvent(FEvent* event) override;

        FListView* listView = nullptr;

    public: // - Fusion Properties - 


        FUSION_WIDGET;
        friend class FListItemStyle;
        friend class FListView;
    };
    
}

#include "FListItemWidget.rtti.h"
