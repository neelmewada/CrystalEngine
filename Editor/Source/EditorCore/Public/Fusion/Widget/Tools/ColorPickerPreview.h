#pragma once

namespace CE::Editor
{
    CLASS()
    class EDITORCORE_API ColorPickerPreview : public FWidget
    {
        CE_CLASS(ColorPickerPreview, FWidget)
    protected:

        ColorPickerPreview();

        void Construct() override;

    public: // - Public API -

        void CalculateIntrinsicSize() override;

    protected: // - Internal -

        void OnPaint(FPainter* painter) override;

    public: // - Fusion Properties - 

        FUSION_PROPERTY(Color, PreviewColor);

        FUSION_WIDGET;
    };
    
}

#include "ColorPickerPreview.rtti.h"
