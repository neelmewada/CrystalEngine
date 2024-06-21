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

    protected: // - Fusion Fields -

        FIELD()
        String m_Text;

        FIELD()
        FFont m_Font;

        FIELD()
        Color m_Foreground = Color::White();

        FIELD()
        FWordWrap m_WordWrap = FWordWrap::Normal;

    public: // - Fusion Properties -

        FUSION_LAYOUT_PROPERTY(Text);
        FUSION_LAYOUT_PROPERTY(Font);
        FUSION_LAYOUT_PROPERTY(WordWrap);

        Self& FontFamily(const CE::Name& fontFamily);
        Self& FontSize(int fontSize);
        Self& Bold(bool bold);
        Self& Italic(bool italic);

        FUSION_PROPERTY(Foreground);

        FUSION_WIDGET;
    };
    
}

#include "FLabel.rtti.h"
