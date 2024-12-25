#include "EditorCore.h"

namespace CE::Editor
{

    ArrayPropertyEditor::ArrayPropertyEditor()
    {

    }

    void ArrayPropertyEditor::OnBeginDestroy()
    {
	    Super::OnBeginDestroy();


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

    void ArrayPropertyEditor::InitTarget(const Array<WeakRef<Object>>& targets, const String& relativeFieldPath)
    {
        right->DestroyAllChildren();

        auto printError = [&](const String& msg)
            {
                right->AddChild(
                    FNew(FLabel)
                    .FontSize(10)
                    .Text("Error: " + msg)
                    .Foreground(Color::Red())
                );
            };

        if (targets.GetSize() > 1)
        {
            printError("Multiple objects selected!");
            return;
        }
        if (targets.GetSize() == 0)
        {
            printError("No objects selected!");
            return;
        }

        Ref<Object> target;

        for (const auto& object : targets)
        {
            if (auto lock = object.Lock())
            {
                target = lock;
                break;
            }
        }

        if (target.IsNull())
        {
            printError("No objects selected!");
            return;
        }

        Ptr<FieldType> field = nullptr;
        Ref<Object> outObject = nullptr;
        void* outInstance = nullptr;

        bool foundField = target->GetClass()->FindFieldInstanceRelative(relativeFieldPath, target,
            field, outObject, outInstance);

        if (!foundField)
        {
            printError("Cannot find field!");
            return;
        }

        fieldName = field->GetName();
        this->relativeFieldPath = relativeFieldPath;

        FieldNameText(field->GetDisplayName());

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

    void ArrayPropertyEditor::UpdateTarget(const Array<WeakRef<Object>>& targets, const String& relativeFieldPath)
    {
        this->targets = targets;
        this->relativeFieldPath = relativeFieldPath;
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

        Ref<Object> target;

        for (const auto& object : targets)
        {
            if (auto lock = object.Lock())
            {
                target = lock;
                break;
            }
        }

        if (target.IsNull())
        {
            return;
        }

        Ref<Object> targetObject;
        Ptr<FieldType> field;
        void* instance = nullptr;

        bool success = target->GetClass()->FindFieldInstanceRelative(relativeFieldPath, targetObject, field, instance);
        if (!success)
        {
	        return;
        }

        int arraySize = field->GetArraySize(instance);

        countLabel->Text(String::Format("{} array elements", arraySize));

        if (arraySize != arrayElements.GetSize())
        {
	        arrayElements = field->GetArrayFieldListPtr(instance);
        }

        bool newElementsCreated = false;

        static FBrush deleteIcon = FBrush("/Engine/Resources/Icons/Delete");
        constexpr f32 iconSize = 16;

	    for (int i = 0; i < arraySize; ++i)
	    {
            const Array<u8>& arrayValue = field->GetFieldValue<Array<u8>>(instance);
            auto arrayInstance = (void*)arrayValue.GetData();
            String arrayElementFieldPath = String::Format("{}[{}]", field->GetName(), i);

            if (i >= elementEditors.GetSize())
            {
	            // Create editor
                PropertyEditor* propertyEditor = PropertyEditorRegistry::Get()->Create(arrayElements[i], objectEditor);

                propertyEditor->SetIndentationLevel(GetIndentationLevel() + 1);

                propertyEditor->InitTarget({ target }, arrayElementFieldPath);

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
	    	elementEditors[i]->UpdateTarget({ target }, arrayElementFieldPath);

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
        WeakRef<Self> self = this;

        //field->InsertArrayElement(instance);

        UpdateValue();
    }

    void ArrayPropertyEditor::DeleteAllElements()
    {
        /*while (field->GetArraySize(instance) > 0)
        {
            field->DeleteArrayElement(instance, 0);
        }*/

        UpdateValue();
    }

    void ArrayPropertyEditor::DeleteElement(u32 index)
    {
        /*if (index < field->GetArraySize(instance))
        {
            field->DeleteArrayElement(instance, index);
        }*/

        UpdateValue();
    }
}

