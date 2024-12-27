#pragma once

namespace CE::Editor
{
    CLASS()
    class EDITORCORE_API TextEditorField : public EditorField
    {
        CE_CLASS(TextEditorField, EditorField)
    protected:

        TextEditorField();

        void Construct() override;

    public: // - Public API -

        EditorField& FixedInputWidth(f32 width) override;

    protected: // - Internal -

        bool CanBind(FieldType* field) override;

        void UpdateValue() override;

        FUNCTION()
        void OnTextFieldEdited(FTextInput* input);

        FUNCTION()
        void OnFinishEdit(FTextInput* input);

        FUNCTION()
        void OnBeginEditing(FTextInput* input);

        FTextInput* input = nullptr;
        String initialValue;

    public: // - Fusion Properties - 


        FUSION_WIDGET;
    };
    
}

#include "TextEditorField.rtti.h"
