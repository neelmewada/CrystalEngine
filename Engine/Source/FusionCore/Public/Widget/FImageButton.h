#pragma once

namespace CE
{
    CLASS()
    class FUSIONCORE_API FImageButton : public FButton
    {
        CE_CLASS(FImageButton, FButton)
    protected:

        FImageButton();

        void Construct() override;

    public: // - Public API -

        FImage* GetImage() const { return image; }

    protected:

        FImage* image = nullptr;

    public: // - Fusion Properties -

        FUSION_PROPERTY_WRAPPER2(Background, image, Image);

        Self& ImageWidth(f32 width);
        Self& ImageHeight(f32 height);

        FUSION_WIDGET;
    };
    
}

#include "FImageButton.rtti.h"
