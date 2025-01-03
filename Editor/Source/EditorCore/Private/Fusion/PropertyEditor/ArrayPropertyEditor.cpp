#include "EditorCore.h"

namespace CE::Editor
{
    static constexpr const char OperationName[] = "Delete Array Element";

    static HashSet<TypeId> serializableOpaqueTypes = {
        TYPEID(u8), TYPEID(s8),
        TYPEID(u16), TYPEID(s16),
        TYPEID(u32), TYPEID(s32),
        TYPEID(u64), TYPEID(s64),
        TYPEID(f32), TYPEID(f64),
        TYPEID(Uuid),
        TYPEID(String), TYPEID(CE::Name),
        TYPEID(Vec2), TYPEID(Vec2i),
        TYPEID(Vec3), TYPEID(Vec3i),
        TYPEID(Vec4), TYPEID(Vec4i),
        TYPEID(Color),
        TYPEID(ClassType), TYPEID(StructType),
        TYPEID(EnumType),
        TYPEID(SubClassType<>)
    };

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
        this->target = target;
        this->targets = targets;

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

        Ptr<FieldType> field;
        void* instance = nullptr;

        bool success = target->GetClass()->FindFieldInstanceRelative(relativeFieldPath, target, field, instance);
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
            Ref<PropertyEditor> editor = elementEditors[i];
            expansionStack->RemoveChild(editor.Get());
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
        Ref<EditorHistory> history = objectEditor->GetEditorHistory();
        Ref<Object> target = this->target.Lock();
        WeakRef<Object> targetWeak = target;
        CE::Name arrayFieldPath = relativeFieldPath;

        if (history.IsValid())
        {
            history->PerformOperation("Insert Array Element", target,
                [arrayFieldPath, targetWeak](const Ref<EditorOperation>& operation)
                {
                    if (auto target = targetWeak.Lock())
                    {
                        Ptr<FieldType> arrayField;
                        void* instance = nullptr;
                        bool success = target->GetClass()->FindFieldInstanceRelative(arrayFieldPath, target,
                            arrayField, instance);
                        if (!success || !arrayField->IsArrayField())
                        {
                            return false;
                        }

                        arrayField->InsertArrayElement(instance);
                        operation->SetArrayIndex(arrayField->GetArraySize(instance) - 1);

                        target->OnFieldChanged(arrayField->GetName());

                        return true;
                    }

                    return false;
                },
                [arrayFieldPath, targetWeak](const Ref<EditorOperation>& operation)
                {
                    if (auto target = targetWeak.Lock())
                    {
                        Ptr<FieldType> arrayField;
                        void* instance = nullptr;
                        bool success = target->GetClass()->FindFieldInstanceRelative(arrayFieldPath, target,
                            arrayField, instance);
                        if (!success || !arrayField->IsArrayField())
                        {
                            return false;
                        }

                        int index = operation->GetArrayIndex();

                        if (index < 0 || index >= (int)arrayField->GetArraySize(instance))
                        {
	                        return false;
                        }

                        arrayField->DeleteArrayElement(instance, index);

                        target->OnFieldChanged(arrayField->GetName());

                        return true;
                    }

                    return false;
                });
        }
        else
        {
            Ptr<FieldType> field;
            void* instance = nullptr;
            bool success = target->GetClass()->FindFieldInstanceRelative(relativeFieldPath, target,
                field, instance);
            if (!success)
            {
                return;
            }

            field->InsertArrayElement(instance);
        }

        UpdateValue();
    }

    void ArrayPropertyEditor::DeleteAllElements()
    {
        // TODO: Delete elements in a way that they can be re-created back to the same state.
        // Use serialization

        Ref<EditorHistory> history = objectEditor->GetEditorHistory();
        Ref<Object> target = this->target.Lock();
        WeakRef<Object> targetWeak = target;
        CE::Name arrayFieldPath = relativeFieldPath;

        if (history.IsValid())
        {
            Ptr<FieldType> field;
            void* instance = nullptr;
            bool success = target->GetClass()->FindFieldInstanceRelative(relativeFieldPath, target,
                field, instance);
            if (!success)
            {
                return;
            }

            TypeInfo* underlyingType = field->GetUnderlyingType();
            if (underlyingType == nullptr)
            {
	            return;
            }

            TypeId underlyingTypeId = underlyingType->GetTypeId();

            if (underlyingType->IsObject())
            {
	            
            }
            else if (underlyingType->IsStruct())
            {
	            
            }
            else if (underlyingType->IsEnum())
            {
	            
            }
            else if (underlyingType->GetTypeId() == TYPEID(ScriptEvent<>))
            {
	            
            }
            else if (underlyingType->GetTypeId() == TYPEID(ScriptDelegate<>))
            {

            }
            else // POD/Opaque types
            {
                if (serializableOpaqueTypes.Exists(underlyingTypeId))
                {
                    // Serialize entire array as a 'snapshot'

                    MemoryStream jsonData = MemoryStream(1024, true);
                    jsonData.SetAsciiMode(true);
                    jsonData.SetAutoResizeIncrement(1024);

                    JsonFieldSerializer serializer{ { field }, instance };
                    serializer.Serialize(&jsonData);
                    jsonData.Seek(0);

                    history->PerformOperation(OperationName, target,
                        [targetWeak, arrayFieldPath]
                        (const Ref<EditorOperation>& operation)
                        {
                            if (auto target = targetWeak.Lock())
                            {
                                Ptr<FieldType> field;
                                void* instance = nullptr;
                                bool success = target->GetClass()->FindFieldInstanceRelative(arrayFieldPath, target,
                                    field, instance);
                                if (!success)
                                    return false;

                                while (field->GetArraySize(instance) > 0)
                                {
                                    field->DeleteArrayElement(instance, 0);
                                }

                                target->OnFieldChanged(field->GetName());

                                return true;
                            }

                            return false;
                        },
                        [targetWeak, arrayFieldPath, jsonData]
                        (const Ref<EditorOperation>& operation)
                        {
                            if (auto target = targetWeak.Lock())
                            {
                                Ptr<FieldType> field;
                                void* instance = nullptr;
                                bool success = target->GetClass()->FindFieldInstanceRelative(arrayFieldPath, target,
                                    field, instance);
                                if (!success)
                                    return false;

                                MemoryStream& json = const_cast<MemoryStream&>(jsonData);
                                json.Seek(0);

                                JsonFieldDeserializer deserializer{ { field }, instance };
                                deserializer.Deserialize(&json);

                                target->OnFieldChanged(field->GetName());

                                return true;
                            }

                            return false;
                        });
                }
            }
        }
        else
        {
            Ptr<FieldType> field;
            void* instance = nullptr;
            bool success = target->GetClass()->FindFieldInstanceRelative(relativeFieldPath, target,
                field, instance);
            if (!success)
            {
                return;
            }

            while (field->GetArraySize(instance) > 0)
            {
                field->DeleteArrayElement(instance, 0);
            }
        }

        UpdateValue();
    }

    void ArrayPropertyEditor::DeleteElement(u32 index)
    {
        Ref<EditorHistory> history = objectEditor->GetEditorHistory();
        Ref<Object> target = this->target.Lock();
        WeakRef<Object> targetWeak = target;
        CE::Name arrayFieldPath = relativeFieldPath;

        if (history.IsValid())
        {
            Ptr<FieldType> field;
            void* instance = nullptr;
            bool success = target->GetClass()->FindFieldInstanceRelative(relativeFieldPath, target,
                field, instance);
            if (!success || !field->IsArrayField())
                return;

            Ptr<FieldType> elementField = field->GetArrayFieldElementPtr(instance, index);

            if (elementField == nullptr)
                return;

            TypeInfo* declType = elementField->GetDeclarationType();
            if (declType == nullptr)
                return;

            TypeId declId = declType->GetTypeId();

            void* arrayInstance = field->GetArrayInstance(instance);

            if (declType->IsObject())
            {
                // Serialize entire array as a 'snapshot'

                MemoryStream jsonData = MemoryStream(1024, true);
                jsonData.SetAsciiMode(true);
                jsonData.SetAutoResizeIncrement(1024);

                JsonFieldSerializer serializer{ { field }, instance };
                serializer.Serialize(&jsonData);
                jsonData.Seek(0);

                history->PerformOperation(OperationName, target,
                    [targetWeak, index, arrayFieldPath]
                    (const Ref<EditorOperation>& operation)
                    {
                        if (auto target = targetWeak.Lock())
                        {
                            Ptr<FieldType> field;
                            void* instance = nullptr;
                            bool success = target->GetClass()->FindFieldInstanceRelative(arrayFieldPath, target,
                                field, instance);
                            if (!success)
                                return false;
                            if (index >= field->GetArraySize(instance))
                                return false;

                            field->DeleteArrayElement(instance, index);

                            target->OnFieldChanged(field->GetName());

                            return true;
                        }

                        return false;
                    },
                    [targetWeak, jsonData, arrayFieldPath]
                    (const Ref<EditorOperation>& operation)
                    {
                        if (auto target = targetWeak.Lock())
                        {
                            Ptr<FieldType> field;
                            void* instance = nullptr;
                            bool success = target->GetClass()->FindFieldInstanceRelative(arrayFieldPath, target,
                                field, instance);
                            if (!success)
                                return false;

                            MemoryStream& json = const_cast<MemoryStream&>(jsonData);
                            json.Seek(0);

                            JsonFieldDeserializer deserializer{ { field }, instance };
                            deserializer.Deserialize(&json);

                            target->OnFieldChanged(field->GetName());

                            return true;
                        }

                        return false;
                    });
            }
            else if (declType->IsStruct())
            {
                StructType* structType = static_cast<StructType*>(declType);
                void* structInstance = elementField->GetFieldInstance(arrayInstance);

                MemoryStream jsonData = MemoryStream(1024, true);
                jsonData.SetAsciiMode(true);
                jsonData.SetAutoResizeIncrement(1024);

                JsonFieldSerializer serializer{ { field }, instance };
                serializer.Serialize(&jsonData);
                jsonData.Seek(0);

                history->PerformOperation(OperationName, target,
                    [targetWeak, index, arrayFieldPath](const Ref<EditorOperation>& operation)
                    {
                        if (auto target = targetWeak.Lock())
                        {
                            Ptr<FieldType> field;
                            void* instance = nullptr;
                            bool success = target->GetClass()->FindFieldInstanceRelative(arrayFieldPath, target,
                                field, instance);
                            if (!success)
                                return false;

                            field->DeleteArrayElement(instance, index);

                            target->OnFieldChanged(field->GetName());

                            return true;
                        }

                        return false;
                    },
                    [targetWeak, jsonData, index, arrayFieldPath, structType](const Ref<EditorOperation>& operation)
                    {
                        if (auto target = targetWeak.Lock())
                        {
                            Ptr<FieldType> field;
                            void* instance = nullptr;
                            bool success = target->GetClass()->FindFieldInstanceRelative(arrayFieldPath, target,
                                field, instance);
                            if (!success)
                                return false;

                            MemoryStream& json = const_cast<MemoryStream&>(jsonData);
                            json.Seek(0);

                            JsonFieldDeserializer deserializer{ { field }, instance };
                            deserializer.Deserialize(&json);

                            target->OnFieldChanged(field->GetName());

                            return true;
                        }

                        return false;
                    });
            }
            else if (declType->IsEnum())
            {
                s64 enumValue = elementField->GetFieldEnumValue(arrayInstance);

                history->PerformOperation(OperationName, target,
                    [targetWeak, index, arrayFieldPath, enumValue](const Ref<EditorOperation>& operation)
                    {
                        if (auto target = targetWeak.Lock())
                        {
                            Ptr<FieldType> field;
                            void* instance = nullptr;
                            bool success = target->GetClass()->FindFieldInstanceRelative(arrayFieldPath, target,
                                field, instance);
                            if (!success)
                                return false;
                            if (index >= field->GetArraySize(instance))
                                return false;

                            field->DeleteArrayElement(instance, index);

                            target->OnFieldChanged(field->GetName());

                            return true;
                        }
                        
                        return false;
                    },
                    [targetWeak, index, arrayFieldPath, enumValue](const Ref<EditorOperation>& operation)
                    {
                        if (auto target = targetWeak.Lock())
                        {
                            Ptr<FieldType> field;
                            void* instance = nullptr;
                            bool success = target->GetClass()->FindFieldInstanceRelative(arrayFieldPath, target,
                                field, instance);
                            if (!success)
                                return false;
                            if (index >= field->GetArraySize(instance) + 1)
                                return false;

                            field->InsertArrayElement(instance, index);
                            void* arrayInstance = field->GetArrayInstance(instance);

                            Ptr<FieldType> elementField = field->GetArrayFieldElementPtr(instance, index);
                            elementField->SetFieldEnumValue(arrayInstance, enumValue);

                            target->OnFieldChanged(field->GetName());

                            return true;
                        }

                        return false;
                    });
            }
            else if (declId == TYPEID(ScriptEvent<>))
            {
	            
            }
            else if (declId == TYPEID(ScriptDelegate<>))
            {
	            
            }
            else // POD/Opaque type
            {
                if (serializableOpaqueTypes.Exists(declId))
                {
                    // Serialize entire array as a 'snapshot'

                    MemoryStream jsonData = MemoryStream(1024, true);
                    jsonData.SetAsciiMode(true);
                    jsonData.SetAutoResizeIncrement(1024);

                    JsonFieldSerializer serializer{ { field }, instance };
                    serializer.Serialize(&jsonData);
                    jsonData.Seek(0);

                    history->PerformOperation(OperationName, target,
                        [targetWeak, index, arrayFieldPath]
                        (const Ref<EditorOperation>& operation)
                        {
                            if (auto target = targetWeak.Lock())
                            {
                                Ptr<FieldType> field;
                                void* instance = nullptr;
                                bool success = target->GetClass()->FindFieldInstanceRelative(arrayFieldPath, target,
                                    field, instance);
                                if (!success)
                                    return false;
                                if (index >= field->GetArraySize(instance))
                                    return false;

                                field->DeleteArrayElement(instance, index);

                                target->OnFieldChanged(field->GetName());

                                return true;
                            }

                            return false;
                        },
                        [targetWeak, index, arrayFieldPath, jsonData]
                        (const Ref<EditorOperation>& operation)
                        {
                            if (auto target = targetWeak.Lock())
                            {
                                Ptr<FieldType> field;
                                void* instance = nullptr;
                                bool success = target->GetClass()->FindFieldInstanceRelative(arrayFieldPath, target,
                                    field, instance);
                                if (!success)
                                    return false;

                                MemoryStream& json = const_cast<MemoryStream&>(jsonData);
                                json.Seek(0);

                                JsonFieldDeserializer deserializer{ { field }, instance };
                                deserializer.Deserialize(&json);

                                target->OnFieldChanged(field->GetName());

                                return true;
                            }

                            return false;
                        });
                }
            }
        }
        else
        {
            Ptr<FieldType> field;
            void* instance = nullptr;
            bool success = target->GetClass()->FindFieldInstanceRelative(relativeFieldPath, target,
                field, instance);
            if (!success)
            {
                return;
            }

            if (index < field->GetArraySize(instance))
            {
                field->DeleteArrayElement(instance, index);
            }
        }

        UpdateValue();
    }

}

