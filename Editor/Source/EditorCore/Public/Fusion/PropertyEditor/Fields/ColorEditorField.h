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

        void HandleEvent(FEvent* event) override;

    public: // - Public API -

        bool CanBind(FieldType* field) override;

        bool SupportsMouseEvents() const override { return true; }

        void SetColorValue(const Color& color);

    protected: // - Internal -

        void UpdateValue() override;

        Color value;
        bool isMouseInside = false;
        bool isPressed = false;

        FStyledWidget* left = nullptr;
        FStyledWidget* right = nullptr;

    public: // - Fusion Properties - 


        FUSION_WIDGET;
    };
    
}

#include "ColorEditorField.rtti.h"
