#pragma once

namespace CE
{
    
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

        ObjectEditor* FindOrCreate(Object* targetObject);
        ObjectEditor* FindOrCreate(const Array<Object*>& targetObjects);

        template<typename T> requires !TIsSameType<Object, T>::Value and TIsBaseClassOf<Object, T>::Value
        ObjectEditor* FindOrCreate(const Array<T*>& targetObjects)
        {
            Array<Object*> objects;
            objects.Resize(targetObjects.GetSize());
            for (int i = 0; i < targetObjects.GetSize(); ++i)
            {
                objects[i] = targetObjects[i];
            }
            return FindOrCreate(objects);
        }

    private:

        SubClass<ObjectEditor> FindEditorClass(Object* targetObject);

        HashMap<TypeId, SubClass<ObjectEditor>> customEditorRegistry;

        HashMap<Uuid, ObjectEditor*> objectEditorsByInstances;

        friend class ObjectEditor;
    };
    
} // namespace CE

