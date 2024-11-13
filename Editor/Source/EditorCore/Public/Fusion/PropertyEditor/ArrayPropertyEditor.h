#pragma once

namespace CE::Editor
{
    CLASS()
    class EDITORCORE_API ArrayPropertyEditor : public PropertyEditor
    {
        CE_CLASS(ArrayPropertyEditor, PropertyEditor)
    protected:

        ArrayPropertyEditor();

        void OnBeforeDestroy() override;

        void ConstructEditor() override;

        void InitTarget(FieldType* field, const Array<Object*>& targets, const Array<void*>& instances) override;

        void UpdateTarget(FieldType* field, const Array<Object*>& targets, const Array<void*>& instances) override;

    public: // - Public API -

        void SetSplitRatio(f32 ratio, FSplitBox* excluding) override;

        bool IsFieldSupported(FieldType* field) const override;

        bool IsFieldSupported(TypeId fieldTypeId) const override;

        bool IsExpandable() override;

        void UpdateValue() override;

        FUNCTION()
        void InsertElement();

        FUNCTION()
        void DeleteAllElements();

        FUNCTION()
        void DeleteElement(u32 index);


    protected: // - Internal -

        FLabel* countLabel = nullptr;

        Object* target = nullptr;
        void* instance = nullptr;

        Array<FieldType> arrayElements;
        Array<PropertyEditor*> elementEditors;

    public: // - Fusion Properties - 


        FUSION_WIDGET;
    };
    
}

#include "ArrayPropertyEditor.rtti.h"
