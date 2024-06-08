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


    protected:

        void Construct() override;

    private:  // - Fusion Fields -

        FIELD()
        FStackBoxDirection m_Direction = FStackBoxDirection::Horizontal;

        FIELD()
        CE::HAlign m_ContentHAlign = HAlign::Fill;

        FIELD()
        CE::VAlign m_ContentVAlign = VAlign::Fill;

    public:  // - Fusion Properties -

        FUSION_PROPERTY(Direction);

        FUSION_PROPERTY(ContentHAlign);

        FUSION_PROPERTY(ContentVAlign);

        FUSION_TESTS;
    };
    
} // namespace CE

#include "FStackBox.rtti.h"