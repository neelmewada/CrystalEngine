#pragma once

namespace CE
{

    ENUM()
    enum class FStackBoxDirection
    {
	    Horizontal,
        Vertical
    };
    ENUM_CLASS(FStackBoxDirection)

    CLASS()
    class FUSIONCORE_API FStackBox : public FContainerWidget
    {
        CE_CLASS(FStackBox, FContainerWidget)
    public:

        FStackBox();

        virtual ~FStackBox();

        // - Public API -

        void CalculateIntrinsicSize() override;

        void PlaceSubWidgets() override;

        void OnPaint(FPainter* painter) override;

    protected:

        void Construct() override;

    public:  // - Fusion Properties -

        FUSION_LAYOUT_PROPERTY(FStackBoxDirection, Direction);
        FUSION_LAYOUT_PROPERTY(CE::HAlign, ContentHAlign);
        FUSION_LAYOUT_PROPERTY(CE::VAlign, ContentVAlign);
        FUSION_LAYOUT_PROPERTY(f32, Gap);

        FUSION_WIDGET;
    };

    CLASS()
    class FUSIONCORE_API FVerticalStack : public FStackBox
    {
        CE_CLASS(FVerticalStack, FStackBox)
    public:

        FVerticalStack()
        {
            m_Direction = FStackBoxDirection::Vertical;
        }


        FUSION_WIDGET;
    };

    CLASS()
    class FUSIONCORE_API FHorizontalStack : public FStackBox
    {
        CE_CLASS(FHorizontalStack, FStackBox)
    public:

        FHorizontalStack()
        {
            m_Direction = FStackBoxDirection::Horizontal;
        }


        FUSION_WIDGET;
    };

} // namespace CE

#include "FStackBox.rtti.h"