#include "EditorCore.h"

namespace CE::Editor
{

    BoolEditorField::BoolEditorField()
    {

    }

    void BoolEditorField::Construct()
    {
        Super::Construct();

        Child(
            FAssignNew(FCheckbox, checkbox)
            .OnCheckChanged(FUNCTION_BINDING(this, OnCheckChanged))
            .HAlign(HAlign::Left)
            .VAlign(VAlign::Center)
        );

    }

    bool BoolEditorField::CanBind(FieldType* field)
    {
        return field->GetDeclarationTypeId() == TYPEID(bool);
    }

    void BoolEditorField::UpdateValue()
    {
        if (!IsBound())
            return;

        Object* target = targets[0];

        checkbox->Checked(field->GetFieldValue<bool>(target));
    }

    void BoolEditorField::OnCheckChanged(FCheckbox* checkbox)
    {
        if (!IsBound())
            return;

        Object* target = targets[0];

        field->SetFieldValue<bool>(target, checkbox->Checked());
        target->OnFieldEdited(field->GetName());
    }
}

