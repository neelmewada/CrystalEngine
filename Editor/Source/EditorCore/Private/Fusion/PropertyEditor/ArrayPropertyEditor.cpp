#include "EditorCore.h"

namespace CE::Editor
{

    ArrayPropertyEditor::ArrayPropertyEditor()
    {

    }

    void ArrayPropertyEditor::OnBeforeDestroy()
    {
	    Super::OnBeforeDestroy();

        if (IsValidObject(target))
        {
            ObjectListener::RemoveListener(target, this);
        }
    }

    void ArrayPropertyEditor::ConstructEditor()
    {
        ConstructDefaultEditor();

        expansionArrow->Visible(true);

        (*expansionStack)
            (
                FNew(FLabel)
            );
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

        if (this->target != nullptr)
        {
            ObjectListener::RemoveListener(this->target, this);
        }

        this->field = field;
        target = targets[0];

        ObjectListener::AddListener(this->target, this);

        right->DestroyAllChildren();

        right->AddChild(
            FNew(FLabel)
            .Text(String::Format("{} Elements", field->GetArraySize(target)))
        );

    }

    void ArrayPropertyEditor::OnObjectFieldChanged(Object* object, const CE::Name& fieldName)
    {
	    if (target != object || target == nullptr || field == nullptr || field->GetName() != fieldName)
            return;

        
    }

    bool ArrayPropertyEditor::IsExpandable()
    {
        return true;
    }

}

