#pragma once

namespace CE::Editor
{
    CLASS()
    class EDITORCORE_API ColorEditorField : public EditorField
    {
        CE_CLASS(ColorEditorField, EditorField)
    protected:

        ColorEditorField();

        void Construct() override;

        void OnPaint(FPainter* painter) override;

    public: // - Public API -

        bool CanBind(FieldType* field) override;

    protected: // - Internal -

        void UpdateValue() override;

        Color value;

    public: // - Fusion Properties - 


        FUSION_WIDGET;
    };
    
}

#include "ColorEditorField.rtti.h"
