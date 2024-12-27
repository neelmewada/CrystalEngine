#pragma once

namespace CE::Editor
{
    CLASS()
    class EDITORCORE_API ArrayPropertyEditor : public PropertyEditor
    {
        CE_CLASS(ArrayPropertyEditor, PropertyEditor)
    protected:

        ArrayPropertyEditor();

        void OnBeginDestroy() override;

        void ConstructEditor() override;

        void InitTarget(const Array<WeakRef<Object>>& targets, const String& relativeFieldPath) override;

        void UpdateTarget(const Array<WeakRef<Object>>& targets, const String& relativeFieldPath) override;

    public: // - Public API -

        void SetSplitRatio(f32 ratio, FSplitBox* excluding) override;

        bool IsFieldSupported(FieldType* field) const override;

        bool IsFieldSupported(TypeId fieldTypeId) const override;

        bool IsExpandable() override;

        void UpdateValue() override;

        PropertyEditor& FixedInputWidth(f32 width) override;

        FUNCTION()
        void InsertElement();

        FUNCTION()
        void DeleteAllElements();

        FUNCTION()
        void DeleteElement(u32 index);


    protected: // - Internal -

        FLabel* countLabel = nullptr;
        f32 fixedInputWidth = -1;

        Array<Ptr<FieldType>> arrayElements;
        Array<PropertyEditor*> elementEditors;

    public: // - Fusion Properties - 


        FUSION_WIDGET;
    };
    
}

#include "ArrayPropertyEditor.rtti.h"
