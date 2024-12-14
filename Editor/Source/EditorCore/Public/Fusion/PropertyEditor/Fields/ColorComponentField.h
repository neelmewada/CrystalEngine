#pragma once

namespace CE::Editor
{
    CLASS()
    class EDITORCORE_API ColorComponentField : public NumericEditorField
    {
        CE_CLASS(ColorComponentField, NumericEditorField)
    protected:

        ColorComponentField();

        void Construct() override;

    public: // - Public API -


    protected: // - Internal -

        void OnPaintBeforeText(FPainter* painter) override;

    public: // - Fusion Properties - 

        FUSION_PROPERTY(FGradient, Gradient);
        FUSION_PROPERTY(bool, Transparency);

        FUSION_WIDGET;
    };
    
}

#include "ColorComponentField.rtti.h"
