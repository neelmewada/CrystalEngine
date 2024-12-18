#include "EditorCore.h"

namespace CE::Editor
{

    TextEditorField::TextEditorField()
    {

    }

    void TextEditorField::Construct()
    {
        Super::Construct();

        Child(
            FAssignNew(FTextInput, input)
            .OnTextEditingFinished(FUNCTION_BINDING(this, OnFinishEdit))
            .OnTextEdited(FUNCTION_BINDING(this, OnTextFieldEdited))
            .Width(100)
            .VAlign(VAlign::Center)
        );
    }

    EditorField& TextEditorField::FixedInputWidth(f32 width)
    {
        input->Width(width);
        return *this;
    }

    bool TextEditorField::CanBind(FieldType* field)
    {
        thread_local HashSet types = { TYPEID(String), TYPEID(CE::Name) };

        return types.Exists(field->GetDeclarationTypeId());
    }

    void TextEditorField::UpdateValue()
    {
        if (!IsBound())
            return;

        Object* target = targets[0];

        TypeId fieldDeclId = field->GetDeclarationTypeId();

        const String& string = field->GetFieldValueAsString(instances[0]);

        input->Text(string);
    }

    void TextEditorField::OnTextFieldEdited(FTextInput*)
    {
        if (!IsBound())
            return;

        Object* target = targets[0];

        TypeId fieldDeclId = field->GetDeclarationTypeId();

        const String& string = input->Text();

        if (fieldDeclId == TYPEID(String))
        {
            field->SetFieldValue<String>(instances[0], string);
            target->OnFieldEdited(field->GetName());
        }
        else if (fieldDeclId == TYPEID(CE::Name))
        {
            field->SetFieldValue<CE::Name>(instances[0], string);
            target->OnFieldEdited(field->GetName());
        }
    }

    void TextEditorField::OnFinishEdit(FTextInput*)
    {
        if (!IsBound())
            return;

        Object* target = targets[0];

        TypeId fieldDeclId = field->GetDeclarationTypeId();

        const String& string = input->Text();

        if (fieldDeclId == TYPEID(String))
        {
            field->SetFieldValue<String>(instances[0], string);
            target->OnFieldEdited(field->GetName());
        }
        else if (fieldDeclId == TYPEID(CE::Name))
        {
            field->SetFieldValue<CE::Name>(instances[0], string);
            target->OnFieldEdited(field->GetName());
        }
    }

}

