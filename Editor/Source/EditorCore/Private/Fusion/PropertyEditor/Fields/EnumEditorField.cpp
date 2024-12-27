#include "EditorCore.h"

namespace CE::Editor
{

    EnumEditorField::EnumEditorField()
    {

    }

    void EnumEditorField::Construct()
    {
        Super::Construct();

        Child(
            FAssignNew(FComboBox, comboBox)
            .OnSelectionChanged(FUNCTION_BINDING(this, OnSelectionChanged))
            .FontSize(10)
            .HAlign(HAlign::Left)
            .Width(100)
        );
    }

    bool EnumEditorField::CanBind(FieldType* field)
    {
        return field->IsEnumField() && !field->IsEnumFlagsField();
    }

    void EnumEditorField::UpdateValue()
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

        auto enumType = static_cast<EnumType*>(field->GetDeclarationType());

        if (comboBox->Items().IsEmpty())
        {
        	Array<String> items;

            items.Resize(enumType->GetConstantsCount());

	        for (int i = 0; i < items.GetSize(); ++i)
	        {
                items[i] = enumType->GetConstant(i)->GetDisplayName();
	        }

            if (items.NotEmpty())
            {
                comboBox->Items(items);
                comboBox->SelectItem(0);
            }
            else
            {
	            return;
            }
        }

        if (EnumConstant* constant = enumType->FindConstantWithValue(field->GetFieldEnumValue(outInstance)))
        {
	        for (int i = 0; i < enumType->GetConstantsCount(); ++i)
	        {
		        if (enumType->GetConstant(i) == constant)
		        {
                    comboBox->SelectItem(i);
			        break;
		        }
	        }
        }
        else
        {
            comboBox->SelectItem(0);
            field->SetFieldEnumValue(outInstance, enumType->GetConstant(0)->GetValue());
            target->OnFieldEdited(field->GetName());
        }
    }

    void EnumEditorField::OnSelectionChanged(FComboBox* comboBox)
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

        auto enumType = static_cast<EnumType*>(field->GetDeclarationType());

        int selectedIndex = comboBox->GetSelectedItemIndex();
        if (selectedIndex < 0 || selectedIndex >= enumType->GetConstantsCount())
            return;

        CE::Name fieldPath = relativeFieldPath;

        if (auto history = m_History.Lock())
        {
            Ref<Object> target = targets[0].Lock();
            if (target.IsNull())
                return;

            WeakRef<Object> targetWeak = target;

            s64 initialValue = field->GetFieldEnumValue(outInstance);
            s64 newValue = enumType->GetConstant(selectedIndex)->GetValue();

            history->PerformOperation("Edit Enum Field", target, 
                [targetWeak, fieldPath, newValue]
				(const Ref<EditorOperation>& operation)
                {
                    if (auto target = targetWeak.Lock())
                    {
                        Ptr<FieldType> field;
                        void* instance = nullptr;
                        bool success = target->GetClass()->FindFieldInstanceRelative(fieldPath, target,
                            field, instance);
                        if (!success)
                        {
                            return false;
                        }

                        field->SetFieldEnumValue(instance, newValue);
                        target->OnFieldChanged(field->GetName());

                        return true;
                    }

                    return false;
                },
                [targetWeak, fieldPath, initialValue]
                (const Ref<EditorOperation>& operation)
                {
                    if (auto target = targetWeak.Lock())
                    {
                        Ptr<FieldType> field;
                        void* instance = nullptr;
                        bool success = target->GetClass()->FindFieldInstanceRelative(fieldPath, target,
                            field, instance);
                        if (!success)
                        {
                            return false;
                        }

                        field->SetFieldEnumValue(instance, initialValue);
                        target->OnFieldChanged(field->GetName());

                        return true;
                    }

                    return false;
                });
        }
        else
        {
            field->SetFieldEnumValue(outInstance, enumType->GetConstant(selectedIndex)->GetValue());
            target->OnFieldChanged(field->GetName());
        }
    }
}

