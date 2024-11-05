#pragma once

namespace CE
{
    CLASS()
    class FUSIONCORE_API FTextButton : public FButton
    {
        CE_CLASS(FTextButton, FButton)
    public:

        FTextButton();

    protected:

        void Construct() override final;

    private:

        FIELD()
        FLabel* label = nullptr;

    protected: // - Fusion Fields -


    public: // - Fusion Properties -

        FUSION_PROPERTY_WRAPPER(Text, label);
        FUSION_PROPERTY_WRAPPER(Font, label);
        FUSION_PROPERTY_WRAPPER(FontFamily, label);
        FUSION_PROPERTY_WRAPPER(FontSize, label);
        FUSION_PROPERTY_WRAPPER(Underline, label);
        FUSION_PROPERTY_WRAPPER(Bold, label);
        FUSION_PROPERTY_WRAPPER(Italic, label);
        FUSION_PROPERTY_WRAPPER(WordWrap, label);
        FUSION_PROPERTY_WRAPPER(Foreground, label);
        FUSION_PROPERTY_WRAPPER2(HAlign, label, TextHAlign);
        FUSION_PROPERTY_WRAPPER2(VAlign, label, TextVAlign);

        FUSION_WIDGET;
    };
    
}

#include "FTextButton.rtti.h"
