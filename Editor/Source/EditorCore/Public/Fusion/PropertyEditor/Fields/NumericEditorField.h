#pragma once

namespace CE::Editor
{
    CLASS()
    class EDITORCORE_API NumericEditorField : public EditorField
    {
        CE_CLASS(NumericEditorField, EditorField)
    protected:

        NumericEditorField();

        void Construct() override;

    public: // - Public API -

        bool SupportsMouseEvents() const override { return true; }

        bool SupportsDragEvents() const override { return true; }

        EditorField& FixedInputWidth(f32 width) override;

        void HandleEvent(FEvent* event) override;

    protected: // - Internal -

        FWidget* HitTest(Vec2 localMousePos) override;

        void OnPaint(FPainter* painter) override;

        bool CanBind(FieldType* field) override;

        void UpdateValue() override;

        FUNCTION()
        void OnTextFieldEdited(FTextInput* field);

        FUNCTION()
        void OnFinishEdit(FTextInput* field);

        TypeId numericType = 0;
        FTextInput* input = nullptr;
        bool isCursorPushed = false;
        bool isDragging = false;
        bool isMouseInside = false;
        f32 startMouseX = 0;
        f64 startValue = 0;

    public: // - Fusion Properties - 

        FUSION_PROPERTY(bool, ColorTagVisible);
        FUSION_PROPERTY(Color, ColorTag);
        FUSION_PROPERTY(f32, ScrollAmount);

        FUSION_PROPERTY_WRAPPER(Text, input);

        FUSION_EVENT(ScriptEvent<void(NumericEditorField*)>, OnTextEdited);
        FUSION_EVENT(ScriptEvent<void(NumericEditorField*)>, OnTextEditingFinished);

        template<typename T> requires TIsNumericType<T>::Value
        Self& NumericType()
        {
            numericType = TYPEID(T);
            input->Validator(FNumericInputValidator<T>);
            return *this;
        }

        Self& NumericType(TypeId type);

        FUSION_WIDGET;
    };
    
}

#include "NumericEditorField.rtti.h"
