#pragma once

namespace CE
{
    CLASS()
    class FUSIONCORE_API FLabel : public FWidget
    {
        CE_CLASS(FLabel, FWidget)
    public:

        FLabel();

        void CalculateIntrinsicSize() override;

        void OnPaint(FPainter* painter) override;

    public: // - Fusion Properties -

        FUSION_DATA_LAYOUT_PROPERTY(String, Text);

        FUSION_LAYOUT_PROPERTY(FFont, Font);
        FUSION_LAYOUT_PROPERTY(FWordWrap, WordWrap);

    	FUSION_PROPERTY(Color, Foreground);

        Self& FontFamily(const CE::Name& fontFamily);
        Self& FontSize(int fontSize);
        Self& Bold(bool bold);
        Self& Italic(bool italic);

        const CE::Name& FontFamily();
        const int& FontSize();
        const bool& Bold();
        const bool& Italic();

        FUSION_WIDGET;
    };
    
}

#include "FLabel.rtti.h"
