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

        virtual void SetTarget(FieldType* field, const Array<Object*>& targets);

    public: // - Public API -

        FSplitBox* GetSplitBox() const { return splitBox; }

        //! @brief This function will be called on the class' CDI to check if this property editor
        //! supports multi object editing.
        virtual bool SupportsMultiObjectEditing() const { return false; }

    protected: // - Internal -

        FSplitBox* splitBox = nullptr;

        FHorizontalStack* left = nullptr;
        FHorizontalStack* right = nullptr;

        FLabel* fieldNameLabel = nullptr;

    public: // - Fusion Properties - 

        FUSION_PROPERTY_WRAPPER2(Text, fieldNameLabel, FieldNameText);

        FUSION_WIDGET;
        friend class PropertyEditorRegistry;
        friend class ObjectEditor;
    };
    
}

#include "PropertyEditor.rtti.h"
