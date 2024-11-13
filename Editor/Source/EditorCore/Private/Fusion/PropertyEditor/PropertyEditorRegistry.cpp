#include "EditorCore.h"

namespace CE::Editor
{
    static PropertyEditorRegistry* instance = nullptr;

    PropertyEditorRegistry::PropertyEditorRegistry()
    {
        if (!IsDefaultInstance())
        {
            instance = this;
        }
    }

    PropertyEditorRegistry::~PropertyEditorRegistry()
    {
        
    }

    void PropertyEditorRegistry::OnAfterConstruct()
    {
	    Super::OnAfterConstruct();

        if (IsDefaultInstance())
            return;

        expandedPropertiesHashed.Clear();

        for (const Name& expandedProperty : expandedProperties)
        {
            expandedPropertiesHashed.Add(expandedProperty);
        }
    }

    void PropertyEditorRegistry::OnBeforeDestroy()
    {
	    Super::OnBeforeDestroy();

        if (this == instance)
        {
            expandedProperties.Clear();

            for (const Name& fieldsHashed : expandedPropertiesHashed)
            {
                expandedProperties.Add(fieldsHashed);
            }

            instance = nullptr;
        }
    }

    PropertyEditorRegistry* PropertyEditorRegistry::Get()
    {
        return instance;
    }

    void PropertyEditorRegistry::Register(TypeId fieldTypeId, SubClass<PropertyEditor> editorType)
    {
        customEditorRegistry[fieldTypeId] = editorType;
    }

    void PropertyEditorRegistry::Deregister(TypeId fieldTypeId)
    {
        customEditorRegistry.Remove(fieldTypeId);
    }

    bool PropertyEditorRegistry::IsFieldSupported(TypeId fieldTypeId)
    {
        if (customEditorRegistry.KeyExists(fieldTypeId))
            return true;

        return GetDefaults<PropertyEditor>()->IsFieldSupported(fieldTypeId);
    }

    PropertyEditor* PropertyEditorRegistry::Create(FieldType* field, ObjectEditor* objectEditor)
    {
        TypeInfo* fieldDeclType = field->GetDeclarationType();
        if (!fieldDeclType)
            return nullptr;

        PropertyEditor* editor = nullptr;
        Object* transient = GetTransient(MODULE_NAME);
        ClassType* propertyEditorClass = nullptr;

        if (fieldDeclType->IsClass())
        {
            auto clazz = static_cast<ClassType*>(fieldDeclType);

            while (clazz->GetTypeId() != TYPEID(Object))
            {
                if (customEditorRegistry.KeyExists(clazz->GetTypeId()))
                {
                    propertyEditorClass = customEditorRegistry[clazz->GetTypeId()];
                    if (propertyEditorClass != nullptr)
                    {
                        break;
                    }
                }

                if (clazz->GetSuperClassCount() == 0)
                    break;
                clazz = clazz->GetSuperClass(0);
            }
        }
        else if (customEditorRegistry.KeyExists(fieldDeclType->GetTypeId()))
        {
            propertyEditorClass = customEditorRegistry[fieldDeclType->GetTypeId()];
        }

        if (propertyEditorClass == nullptr)
        {
            propertyEditorClass = PropertyEditor::StaticType();
        }

        if (!static_cast<const PropertyEditor*>(propertyEditorClass->GetDefaultInstance())->IsFieldSupported(field))
        {
            return nullptr;
        }

        editor = CreateObject<PropertyEditor>(transient, "PropertyEditor", OF_NoFlags, propertyEditorClass);
        editor->objectEditor = objectEditor;

        return editor;
    }

    void PropertyEditorRegistry::ExpandField(FieldType* field)
    {
        if (field)
        {
            CE::Name fieldName = field->GetTypeName();

            if (!expandedPropertiesHashed.Exists(fieldName))
            {
                expandedPropertiesHashed.Add(fieldName);
            }
        }
    }

    void PropertyEditorRegistry::CollapseField(FieldType* field)
    {
        if (field)
        {
            CE::Name fieldName = field->GetTypeName();

            expandedPropertiesHashed.Remove(fieldName);
        }
    }

    bool PropertyEditorRegistry::IsExpanded(FieldType* field)
    {
        if (field)
        {
            return expandedPropertiesHashed.Exists(field->GetTypeName());
        }
        return false;
    }

} // namespace CE

