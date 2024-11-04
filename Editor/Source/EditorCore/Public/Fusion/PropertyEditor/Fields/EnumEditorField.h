#pragma once

namespace CE::Editor
{
    CLASS()
    class EDITORCORE_API EnumEditorField : public EditorField
    {
        CE_CLASS(EnumEditorField, EditorField)
    protected:

        EnumEditorField();

        void Construct() override;

    public: // - Public API -



    protected: // - Internal -

        bool CanBind(FieldType* field) override;

        void UpdateValue() override;

        FUNCTION()
        void OnSelectionChanged(FComboBox* comboBox);

        FComboBox* comboBox = nullptr;

    public: // - Fusion Properties - 


        FUSION_WIDGET;
    };
    
}

#include "EnumEditorField.rtti.h"
