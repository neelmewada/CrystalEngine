#include "EditorCore.h"

namespace CE::Editor
{

    PropertyEditorRegistry::PropertyEditorRegistry()
    {

    }

    PropertyEditorRegistry::~PropertyEditorRegistry()
    {
        
    }

    PropertyEditorRegistry& PropertyEditorRegistry::Get()
    {
        static PropertyEditorRegistry instance{};
        return instance;
    }

    PropertyEditor* PropertyEditorRegistry::Create(FieldType* field, Object* target)
    {
        thread_local Array targets = { target };
        targets[0] = target;

        return Create(field, targets);
    }

    PropertyEditor* PropertyEditorRegistry::Create(FieldType* field, const Array<Object*>& targets)
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

        editor = CreateObject<PropertyEditor>(transient, "PropertyEditor", OF_NoFlags, propertyEditorClass);

        return editor;
    }

} // namespace CE

