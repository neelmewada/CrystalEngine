#pragma once

namespace CE::Editor
{
	class PropertyEditorRegistry;

	class EDITORCORE_API ObjectEditorRegistry final
    {
        CE_NO_COPY(ObjectEditorRegistry);
    private:

        ObjectEditorRegistry();
        
    public:

        static ObjectEditorRegistry& Get();

        void Init();
        void Shutdown();

        void RegisterCustomEditor(ClassType* targetClass, const SubClass<ObjectEditor>& editorClass);
        void UnregisterCustomEditor(ClassType* targetClass);

        template<typename TClass>
        void RegisterCustomEditor(const SubClass<ObjectEditor>& editorClass)
        {
            RegisterCustomEditor(TClass::StaticType(), editorClass);
        }

        template<typename TClass>
        void UnregisterCustomEditor()
        {
            UnregisterCustomEditor(TClass::StaticType());
        }

        ObjectEditor* Create(Object* targetObject, const Ref<EditorHistory>& history = nullptr);
        ObjectEditor* Create(const Array<Object*>& targetObjects, const Ref<EditorHistory>& history = nullptr);

        template<typename T> requires (not TIsSameType<Object, T>::Value) and TIsBaseClassOf<Object, T>::Value
        ObjectEditor* Create(const Array<T*>& targetObjects, const Ref<EditorHistory>& history = nullptr)
        {
            Array<Object*> objects;
            objects.Resize(targetObjects.GetSize());
            for (int i = 0; i < targetObjects.GetSize(); ++i)
            {
                objects[i] = targetObjects[i];
            }
            return Create(objects, history);
        }

    private:

        PropertyEditorRegistry* propertyEditorRegistry = nullptr;

        SubClass<ObjectEditor> FindEditorClass(Object* targetObject);

        HashMap<TypeId, SubClass<ObjectEditor>> customEditorRegistry;

        friend class ObjectEditor;
    };
    
} // namespace CE::Editor

