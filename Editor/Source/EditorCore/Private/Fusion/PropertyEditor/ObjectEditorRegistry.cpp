#include "EditorCore.h"

namespace CE::Editor
{

    ObjectEditorRegistry::ObjectEditorRegistry()
    {

    }

    ObjectEditorRegistry& ObjectEditorRegistry::Get()
    {
        static ObjectEditorRegistry instance{};
        return instance;
    }

    void ObjectEditorRegistry::Init()
    {
        propertyEditorRegistry = CreateObject<PropertyEditorRegistry>(nullptr, "PropertyEditorRegistry");

        propertyEditorRegistry->Register(GetTypeId<Array<>>(), GetStaticClass<ArrayPropertyEditor>());
    }

    void ObjectEditorRegistry::Shutdown()
    {
        PropertyEditorRegistry::Get()->Deregister(GetTypeId<Array<>>());

        propertyEditorRegistry->Destroy();
        propertyEditorRegistry = nullptr;
    }

    void ObjectEditorRegistry::RegisterCustomEditor(ClassType* targetClass, const SubClass<ObjectEditor>& editorClass)
    {
        if (targetClass == nullptr || editorClass == nullptr)
            return;

        customEditorRegistry[targetClass->GetTypeId()] = editorClass;
    }

    void ObjectEditorRegistry::UnregisterCustomEditor(ClassType* targetClass)
    {
        if (targetClass == nullptr)
            return;

        customEditorRegistry[targetClass->GetTypeId()] = nullptr;
    }

    ObjectEditor* ObjectEditorRegistry::Create(Object* targetObject)
    {
        thread_local Array<Object*> singleArray = { targetObject };
        singleArray[0] = targetObject;
        return Create(singleArray);
    }

    ObjectEditor* ObjectEditorRegistry::Create(const Array<Object*>& targetObjects)
    {
        if (targetObjects.IsEmpty())
            return nullptr;

        SubClass<ObjectEditor> editorClass = nullptr;

        ObjectEditor* existingEditor = nullptr;

        for (int i = 0; i < targetObjects.GetSize(); ++i)
        {
            Object* object = targetObjects[i];

            if (object == nullptr)
                continue;

            ObjectEditor* found = nullptr;

            if (found && i == 0)
            {
                existingEditor = found;
            }
            else if (existingEditor != found || i == 0)
            {
                existingEditor = nullptr;
                break;
            }
        }

        if (existingEditor)
            return existingEditor;

        for (Object* targetObject : targetObjects)
        {
            if (targetObject == nullptr)
                continue;

            SubClass<ObjectEditor> curEditorClass = FindEditorClass(targetObject);

            if (editorClass != nullptr && curEditorClass != editorClass)
            {
                return nullptr;
            }

            editorClass = curEditorClass;
        }

        if (editorClass == nullptr)
            return nullptr;

        ObjectEditor* editor = CreateObject<ObjectEditor>(GetTransient(MODULE_NAME), "ObjectEditor", OF_NoFlags, editorClass);

        if (targetObjects.GetSize() == 1)
        {
            editor->target = targetObjects[0];
        }

        editor->targets = targetObjects;

        editor->CreateGUI();

        return editor;
    }

    SubClass<ObjectEditor> ObjectEditorRegistry::FindEditorClass(Object* targetObject)
    {
        if (targetObject == nullptr)
            return nullptr;

        SubClass<ObjectEditor> result = GetStaticClass<ObjectEditor>();

        ClassType* classType = targetObject->GetClass();

        while (classType != nullptr)
        {
            if (customEditorRegistry.KeyExists(classType->GetTypeId()))
            {
                result = customEditorRegistry[classType->GetTypeId()];
                break;
            }

            if (classType->GetSuperClassCount() == 0)
                break;
            classType = classType->GetSuperClass(0);
        }

        return result;
    }

} // namespace CE

