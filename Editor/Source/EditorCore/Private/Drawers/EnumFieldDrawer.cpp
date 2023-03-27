
#include "Drawers/EnumFieldDrawer.h"
#include "QtComponents/Input/EnumField.h"

namespace CE::Editor
{   
    
    EnumFieldDrawer::EnumFieldDrawer() : FieldDrawer("EnumFieldDrawer")
    {

    }

    EnumFieldDrawer::~EnumFieldDrawer()
    {

    }

    void EnumFieldDrawer::OnValuesUpdated()
    {
        if (!IsValid())
            return;

        auto type = fieldType->GetDeclarationType();
        if (!type->IsEnum())
            return;

        auto value = fieldType->GetFieldEnumValue(targetInstance);
        enumField->SetValue(value);
    }

    void EnumFieldDrawer::CreateGUI(QLayout* container)
    {
        if (!IsValid())
            return;

        if (enumField != nullptr)
        {
            delete enumField;
        }

        auto declType = fieldType->GetDeclarationType();
        if (declType == nullptr || !declType->IsEnum())
            return;
        auto enumType = (EnumType*)declType;

        enumField = new Qt::EnumField(container->parentWidget());

        enumField->SetEnumType(enumType);
        enumField->SetLabel(fieldType->GetDisplayName());

        container->addWidget(enumField);

        OnValuesUpdated();
        
        auto result = enumField->Bind("OnInputValueChanged", this, "OnInputValueChanged");
    }

    void EnumFieldDrawer::OnInputValueChanged(s64 newValue)
    {
        if (!IsValid())
            return;

        auto type = fieldType->GetDeclarationType();
        if (!type->IsEnum())
            return;

        fieldType->SetFieldEnumValue(targetInstance, newValue);
    }

} // namespace CE::Editor

