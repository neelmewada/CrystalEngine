#pragma once

namespace CE::Editor
{

    CLASS(Prefs = Editor)
    class EDITORCORE_API PropertyEditorRegistry final : public Object
    {
        CE_CLASS(PropertyEditorRegistry, Object)
    private:

        PropertyEditorRegistry();

        ~PropertyEditorRegistry();

        void OnAfterConstruct() override;

        void OnBeginDestroy() override;

    public: // - Public API -

        static PropertyEditorRegistry* Get();

        void Register(TypeId fieldTypeId, SubClass<PropertyEditor> editorType);
        void Deregister(TypeId fieldTypeId);

        bool IsFieldSupported(TypeId fieldTypeId);

        PropertyEditor* Create(FieldType* field, ObjectEditor* objectEditor = nullptr);

        void ExpandField(FieldType* field);
        void CollapseField(FieldType* field);

        bool IsExpanded(FieldType* field);

    private:

        FIELD(Prefs)
        Array<Name> expandedProperties;

        HashSet<Name> expandedPropertiesHashed;

        HashMap<TypeId, SubClass<PropertyEditor>> customEditorRegistry;

        friend class PropertyEditor;
    };
    
} // namespace CE

#include "PropertyEditorRegistry.rtti.h"
