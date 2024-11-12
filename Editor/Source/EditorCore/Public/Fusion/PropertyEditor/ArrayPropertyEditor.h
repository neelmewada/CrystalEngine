#pragma once

namespace CE::Editor
{
    CLASS()
    class EDITORCORE_API ArrayPropertyEditor : public PropertyEditor
    {
        CE_CLASS(ArrayPropertyEditor, PropertyEditor)
    protected:

        ArrayPropertyEditor();

        void ConstructEditor() override;

        bool IsFieldSupported(FieldType* field) const override;

        bool IsFieldSupported(TypeId fieldTypeId) const override;

        void SetTarget(FieldType* field, const Array<Object*>& targets) override;

    public: // - Public API -

        bool IsExpandable() override;

    protected: // - Internal -


    public: // - Fusion Properties - 


        FUSION_WIDGET;
    };
    
}

#include "ArrayPropertyEditor.rtti.h"
