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

        void PrecomputeIntrinsicSize() override;

        void PlaceSubWidgets() override;

    protected:

        void Construct() override;

    protected:  // - Fusion Fields -

        FIELD()
        FStackBoxDirection m_Direction = FStackBoxDirection::Horizontal;

        FIELD()
        CE::HAlign m_ContentHAlign = HAlign::Fill;

        FIELD()
        CE::VAlign m_ContentVAlign = VAlign::Fill;

    public:  // - Fusion Properties -

        FUSION_LAYOUT_PROPERTY(Direction);

        FUSION_LAYOUT_PROPERTY(ContentHAlign);

        FUSION_LAYOUT_PROPERTY(ContentVAlign);

        FUSION_TESTS;
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


        FUSION_TESTS;
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


        FUSION_TESTS;
        FUSION_WIDGET;
    };

} // namespace CE

#include "FStackBox.rtti.h"