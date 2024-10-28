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

    private:

        SubClass<ObjectEditor> FindEditorClass(Object* targetObject);

        HashMap<TypeId, SubClass<ObjectEditor>> customEditorRegistry;

        HashMap<Uuid, ObjectEditor*> objectEditorsByInstances;

    };
    
} // namespace CE

