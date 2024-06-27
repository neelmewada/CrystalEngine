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
        FUSION_PROPERTY_WRAPPER(WordWrap, label);
        FUSION_PROPERTY_WRAPPER(Foreground, label);

        Self& FontFamily(const CE::Name& fontFamily);
        Self& FontSize(int fontSize);
        Self& Bold(bool bold);
        Self& Italic(bool italic);

        FUSION_WIDGET;
    };
    
}

#include "FTextButton.rtti.h"
