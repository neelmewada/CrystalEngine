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

        Ref<Object> target = targets[0].Lock();
        if (target.IsNull())
            return;

        Ptr<FieldType> field = nullptr;
        Ref<Object> outOwner;
        void* outInstance = nullptr;

        bool success = target->GetClass()->FindFieldInstanceRelative(relativeFieldPath, target,
            field, outOwner, outInstance);

        if (!success)
            return;

        checkbox->Checked(field->GetFieldValue<bool>(outInstance));
    }

    void BoolEditorField::OnCheckChanged(FCheckbox* checkbox)
    {
        if (!IsBound())
            return;

        if (auto history = m_History.Lock())
        {
            Ref<Object> target = targets[0].Lock();
            if (target.IsNull())
                return;

            history->PerformOperation<bool>("Edit Bool Field", target, relativeFieldPath,
                !checkbox->Checked(), checkbox->Checked());
        }
        else
        {
            SetValueDirect(checkbox->Checked());
        }
    }
}

