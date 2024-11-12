#pragma once

namespace CE::Editor
{
    CLASS()
    class EDITORCORE_API PropertyEditor : public FCompoundWidget
    {
        CE_CLASS(PropertyEditor, FCompoundWidget)
    protected:

        PropertyEditor();

        void Construct() override;

        void ConstructDefaultEditor();

        virtual void ConstructEditor();

        virtual void SetTarget(FieldType* field, const Array<Object*>& targets);

        void OnPaint(FPainter* painter) override;

    public: // - Public API -

        virtual bool IsFieldSupported(FieldType* field) const;

        virtual bool IsFieldSupported(TypeId fieldTypeId) const;

        virtual bool IsExpandable();

        FSplitBox* GetSplitBox() const { return splitBox; }

        f32 GetSplitRatio() const;

        void SetSplitRatio(f32 ratio);

        Self& FixedInputWidth(f32 width);

        //! @brief This function will be called on the class' CDI to check if this property editor
        //! supports multi object editing.
        virtual bool SupportsMultiObjectEditing() const { return false; }

        FUNCTION()
        void ToggleExpansion();

    protected: // - Internal -

        void UpdateExpansion();

        bool isExpanded = false;

        FSplitBox* splitBox = nullptr;

        FVerticalStack* contentStack = nullptr;

        FImageButton* expansionArrow = nullptr;
        FHorizontalStack* left = nullptr;
        FHorizontalStack* right = nullptr;

        FLabel* fieldNameLabel = nullptr;
        EditorField* editorField = nullptr;
        ObjectEditor* objectEditor = nullptr;

    public: // - Fusion Properties - 

        FUSION_PROPERTY_WRAPPER2(Text, fieldNameLabel, FieldNameText);

        FUSION_PROPERTY(FBrush, HoverRowBackground);

        FUSION_WIDGET;
        friend class PropertyEditorRegistry;
        friend class ObjectEditor;
    };
    
}

#include "PropertyEditor.rtti.h"
