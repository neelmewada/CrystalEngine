#include "EditorSystem.h"

namespace CE::Editor
{

    EnumFieldEditor::EnumFieldEditor()
    {

    }

    EnumFieldEditor::~EnumFieldEditor()
    {
        
    }

    void EnumFieldEditor::BindField(FieldType* field, void* instance)
    {
        if (!field || !instance)
            return;

        if (!field->IsEnumField() && !field->IsEnumFlagsField())
            return;

        EnumType* enumType = (EnumType*)field->GetDeclarationType();
        if (!enumType)
            return;

        s64 enumValue = field->GetFieldEnumValue(instance);

        for (int i = comboBox->GetItemCount() - 1; i >= 0; --i)
        {
            comboBox->RemoveItem(comboBox->GetItem(i));
        }

        int selectedIdx = -1;

        for (int i = 0; i < enumType->GetConstantsCount(); ++i)
        {
            EnumConstant* constant = enumType->GetConstant(i);

            CComboBoxItem* item = CreateObject<CComboBoxItem>(comboBox, "EnumBoxItem");
            item->SetText(constant->GetDisplayName());

            if (constant->GetValue() == enumValue)
            {
                selectedIdx = i;
            }
        }

        if (selectedIdx >= 0)
        {
            comboBox->SelectItem(selectedIdx);
        }

        if (prevHandle)
        {
            comboBox->onSelectionChanged.Unbind(prevHandle);
            prevHandle = 0;
        }
        
        prevHandle = comboBox->onSelectionChanged.Bind([this, enumType, field, instance](int newIndex)
            {
                for (int i = 0; i < enumType->GetConstantsCount(); ++i)
                {
                    EnumConstant* constant = enumType->GetConstant(i);

                    if (i == newIndex)
                    {
                        field->SetFieldEnumValue(instance, constant->GetValue());
                        onValueUpdated();
                        break;
                    }
                }
            });
    }

    void EnumFieldEditor::Construct()
    {
        Super::Construct();

        comboBox = CreateObject<CComboBox>(this, "EnumBox");
    }
    
}

