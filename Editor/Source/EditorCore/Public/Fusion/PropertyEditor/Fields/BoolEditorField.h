#pragma once

namespace CE::Editor
{
    CLASS()
    class EDITORCORE_API BoolEditorField : public EditorField
    {
        CE_CLASS(BoolEditorField, EditorField)
    protected:

        BoolEditorField();

        void Construct() override;

    public: // - Public API -


    protected: // - Internal -

        bool CanBind(FieldType* field) override;

    	void UpdateValue() override;

        FUNCTION()
        void OnCheckChanged(FCheckbox* checkbox);

        FCheckbox* checkbox = nullptr;

    public: // - Fusion Properties - 


        FUSION_WIDGET;
    };
    
}

#include "BoolEditorField.rtti.h"
