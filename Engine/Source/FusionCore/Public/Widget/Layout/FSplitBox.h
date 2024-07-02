#pragma once

namespace CE
{
    ENUM()
    enum class FSplitDirection
    {
	    Horizontal,
        Vertical
    };
    ENUM_CLASS(FSplitDirection);

    CLASS()
    class FUSIONCORE_API FSplitBox : public FContainerWidget
    {
        CE_CLASS(FSplitBox, FContainerWidget)
    public:

        FSplitBox();

        void CalculateIntrinsicSize() override;

        void PlaceSubWidgets() override;

    protected:

        void Construct() override;

        void OnPaint(FPainter* painter) override;

    public: // - Fusion Properties - 

        FUSION_LAYOUT_PROPERTY(FSplitDirection, Direction);
        FUSION_LAYOUT_PROPERTY(CE::HAlign, ContentHAlign);
        FUSION_LAYOUT_PROPERTY(CE::VAlign, ContentVAlign);
        FUSION_LAYOUT_PROPERTY(f32, SplitterSize);

        FUSION_WIDGET;
    };
    
}

#include "FSplitBox.rtti.h"
