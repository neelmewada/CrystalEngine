#pragma once

namespace CE
{
    CLASS()
    class FUSION_API FTreeViewCell : public FCompoundWidget
    {
        CE_CLASS(FTreeViewCell, FCompoundWidget)
    public:

        // - Public API -

    protected:

        FTreeViewCell();

        void Construct() override;

        FImage* arrowIcon = nullptr;
        FLabel* label = nullptr;

    public: // - Fusion Properties -


        FUSION_PROPERTY_WRAPPER(Text, label);
        FUSION_PROPERTY_WRAPPER(FontSize, label);
        FUSION_PROPERTY_WRAPPER(Foreground, label);

        FUSION_PROPERTY_WRAPPER2(Enabled, arrowIcon, ArrowEnabled);
        FUSION_PROPERTY_WRAPPER2(Visible, arrowIcon, ArrowVisible);

        bool ArrowExpanded();
        Self& ArrowExpanded(bool expanded);

        FUSION_WIDGET;
        friend class FTreeViewContainer;
    };
    
}

#include "FTreeViewCell.rtti.h"
