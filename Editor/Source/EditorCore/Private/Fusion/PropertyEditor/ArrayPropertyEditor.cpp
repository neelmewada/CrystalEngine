#include "EditorCore.h"

namespace CE::Editor
{

    ArrayPropertyEditor::ArrayPropertyEditor()
    {

    }

    void ArrayPropertyEditor::ConstructEditor()
    {
        ConstructDefaultEditor();

        expansionArrow->Visible(true);
    }

    bool ArrayPropertyEditor::IsFieldSupported(FieldType* field) const
    {
        if (!field->IsArrayField())
            return false;

        TypeId underlyingTypeId = field->GetUnderlyingTypeId();

        if (underlyingTypeId == TYPEID(Array<>)) // Array of array is not supported
            return false;

        return PropertyEditorRegistry::Get().IsFieldSupported(underlyingTypeId);
    }

    bool ArrayPropertyEditor::IsFieldSupported(TypeId fieldTypeId) const
    {
        return fieldTypeId == TYPEID(Array<>);
    }

    void ArrayPropertyEditor::SetTarget(FieldType* field, const Array<Object*>& targets)
    {
        FieldNameText(field->GetDisplayName());

        Object* target = targets[0];

        right->DestroyAllChildren();

        right->AddChild(
            FNew(FLabel)
            .Text(String::Format("{} Elements", field->GetArraySize(target)))
        );

    }

    bool ArrayPropertyEditor::IsExpandable()
    {
        return true;
    }

}

