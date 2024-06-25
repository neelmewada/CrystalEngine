#pragma once

namespace CE
{
    CLASS()
    class FUSIONCORE_API FImage : public FStyledWidget
    {
        CE_CLASS(FImage, FStyledWidget)
    public:

        FImage();

        // - Public API -

        void CalculateIntrinsicSize() override;

    protected:

        void Construct() override final;

    protected: // - Fusion Fields -


    public: // - Fusion Properties -


        FUSION_WIDGET;
    };
    
}

#include "FImage.rtti.h"
