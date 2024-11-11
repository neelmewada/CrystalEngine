#pragma once

namespace CE::Editor
{

    class EDITORCORE_API PropertyEditorRegistry final
    {
        CE_NO_COPY(PropertyEditorRegistry)
    private:

        PropertyEditorRegistry();

        ~PropertyEditorRegistry();

    public: // - Public API -

        static PropertyEditorRegistry& Get();

        void Register(TypeId fieldTypeId, SubClass<PropertyEditor> editorType);
        void Deregister(TypeId fieldTypeId);

        PropertyEditor* Create(FieldType* field, Object* target);
        PropertyEditor* Create(FieldType* field, const Array<Object*>& targets);

    private:

        HashMap<TypeId, SubClass<PropertyEditor>> customEditorRegistry;

        friend class PropertyEditor;
    };
    
} // namespace CE
