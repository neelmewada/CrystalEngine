#pragma once

namespace CE::Editor
{
    CLASS()
    class EDITORCORE_API ArrayPropertyEditor : public PropertyEditor, IObjectUpdateListener
    {
        CE_CLASS(ArrayPropertyEditor, PropertyEditor)
    protected:

        ArrayPropertyEditor();

        void OnBeforeDestroy() override;

        void ConstructEditor() override;

        void SetTarget(FieldType* field, const Array<Object*>& targets) override;

        void OnObjectFieldChanged(Object* object, const CE::Name& fieldName) override;

    public: // - Public API -

        bool IsFieldSupported(FieldType* field) const override;

        bool IsFieldSupported(TypeId fieldTypeId) const override;

        bool IsExpandable() override;

    protected: // - Internal -

        FieldType* field = nullptr;
        Object* target = nullptr;

    public: // - Fusion Properties - 


        FUSION_WIDGET;
    };
    
}

#include "ArrayPropertyEditor.rtti.h"
