#include "EditorCore.h"

namespace CE::Editor
{

    ArrayPropertyEditor::ArrayPropertyEditor()
    {

    }

    void ArrayPropertyEditor::OnBeforeDestroy()
    {
	    Super::OnBeforeDestroy();


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

        return PropertyEditorRegistry::Get()->IsFieldSupported(underlyingTypeId);
    }

    bool ArrayPropertyEditor::IsFieldSupported(TypeId fieldTypeId) const
    {
        return fieldTypeId == TYPEID(Array<>);
    }

    void ArrayPropertyEditor::InitTarget(FieldType* field, const Array<Object*>& targets, const Array<void*>& instances)
    {
        FieldNameText(field->GetDisplayName());

        this->field = field;
        target = targets[0];
        instance = instances[0];

        right->DestroyAllChildren();

        static FBrush deleteIcon = FBrush("/Engine/Resources/Icons/Delete");
        static FBrush addIcon = FBrush("/Engine/Resources/Icons/Add");
        constexpr f32 iconSize = 16;

        (*right)
        .Gap(10)
        (
            FAssignNew(FLabel, countLabel),

            FNew(FImageButton)
            .Image(addIcon)
            .OnClicked(FUNCTION_BINDING(this, InsertElement))
            .Width(iconSize)
            .Height(iconSize)
            .VAlign(VAlign::Center)
            .Padding(Vec4(1, 1, 1, 1) * 3)
            .Style("Button.Icon"),

            FNew(FImageButton)
            .Image(deleteIcon)
            .OnClicked(FUNCTION_BINDING(this, DeleteAllElements))
            .Width(iconSize)
            .Height(iconSize)
            .VAlign(VAlign::Center)
            .Padding(Vec4(1, 1, 1, 1) * 3)
            .Style("Button.Icon")
        );

        UpdateValue();

        isExpanded = PropertyEditorRegistry::Get()->IsExpanded(field);
        UpdateExpansion();
    }

    void ArrayPropertyEditor::UpdateTarget(FieldType* field, const Array<Object*>& targets, const Array<void*>& instances)
    {
        this->field = field;
        target = targets[0];
        instance = instances[0];
    }

    void ArrayPropertyEditor::SetSplitRatio(f32 ratio, FSplitBox* excluding)
    {
	    Super::SetSplitRatio(ratio, excluding);

        for (PropertyEditor* elementEditor : elementEditors)
        {
            elementEditor->SetSplitRatio(ratio, excluding);
        }
    }

    bool ArrayPropertyEditor::IsExpandable()
    {
        return true;
    }

    void ArrayPropertyEditor::UpdateValue()
    {
	    Super::UpdateValue();

        int arraySize = field->GetArraySize(target);

        countLabel->Text(String::Format("{} Array elements", arraySize));

        CE::Name fieldFullName = field->GetTypeName();

        if (arraySize != arrayElements.GetSize())
        {
	        arrayElements = field->GetArrayFieldList(instance);
        }

        bool newElementsCreated = false;

        static FBrush deleteIcon = FBrush("/Engine/Resources/Icons/Delete");
        constexpr f32 iconSize = 16;

	    for (int i = 0; i < arraySize; ++i)
	    {
            const Array<u8>& arrayValue = field->GetFieldValue<Array<u8>>(instance);
            auto arrayInstance = (void*)arrayValue.GetData();

            if (i >= elementEditors.GetSize())
            {
	            // Create editor
                PropertyEditor* propertyEditor = PropertyEditorRegistry::Get()->Create(&arrayElements[i], objectEditor);

                propertyEditor->SetIndentationLevel(GetIndentationLevel() + 1);

                propertyEditor->InitTarget(&arrayElements[i], { target }, { arrayInstance });

                FHorizontalStack& left = *propertyEditor->GetLeft();
            	FHorizontalStack& right = *propertyEditor->GetRight();

                u32 curIndex = (u32)i;

                if (fixedInputWidth > 0)
                {
                    propertyEditor->FixedInputWidth(fixedInputWidth);
                }

            	right
                (
                    FNew(FImageButton)
                    .Image(deleteIcon)
                    .OnClicked([this, curIndex]
                    {
                        DeleteElement(curIndex);
                    })
                    .Width(iconSize)
                    .Height(iconSize)
                    .VAlign(VAlign::Center)
                    .Padding(Vec4(1, 1, 1, 1) * 3)
                    .Margin(Vec4(5, 0, 5, 0))
                    .Style("Button.Icon")
                );

                elementEditors.Add(propertyEditor);

                expansionStack->AddChild(propertyEditor);

                newElementsCreated = true;
            }

            // Use editor
	    	elementEditors[i]->UpdateTarget(&arrayElements[i], { target }, { arrayInstance });

	    	elementEditors[i]->FieldNameText(String::Format("Index {}", i));

            elementEditors[i]->UpdateValue();
	    }

	    for (int i = (int)elementEditors.GetSize() - 1; i >= arraySize; i--)
	    {
            PropertyEditor* editor = elementEditors[i];
            expansionStack->RemoveChild(editor);
            editor->QueueDestroy();
            elementEditors.RemoveAt(i);
	    }

        if (newElementsCreated && objectEditor)
        {
            objectEditor->ApplySplitRatio(nullptr);
        }
    }

    PropertyEditor& ArrayPropertyEditor::FixedInputWidth(f32 width)
    {
	    Super::FixedInputWidth(width);

        fixedInputWidth = width;

        for (PropertyEditor* elementEditor : elementEditors)
        {
            elementEditor->FixedInputWidth(width);
        }

        return *this;
    }

    void ArrayPropertyEditor::InsertElement()
    {
        field->InsertArrayElement(instance);

        UpdateValue();
    }

    void ArrayPropertyEditor::DeleteAllElements()
    {
        while (field->GetArraySize(instance) > 0)
        {
            field->DeleteArrayElement(instance, 0);
        }

        UpdateValue();
    }

    void ArrayPropertyEditor::DeleteElement(u32 index)
    {
        if (index < field->GetArraySize(instance))
        {
            field->DeleteArrayElement(instance, index);
        }

        UpdateValue();
    }
}

