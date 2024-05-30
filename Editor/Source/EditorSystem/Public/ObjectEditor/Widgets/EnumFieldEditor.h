#pragma once

namespace CE::Editor
{
    CLASS()
    class EDITORSYSTEM_API EnumFieldEditor : public FieldEditor
    {
        CE_CLASS(EnumFieldEditor, FieldEditor)
    public:

        EnumFieldEditor();

        virtual ~EnumFieldEditor();

        void BindField(FieldType* field, void* instance) override;

    protected:

        void Construct() override;

        FIELD()
        CComboBox* comboBox = nullptr;

        DelegateHandle prevHandle = 0;

    };
    
}

#include "EnumFieldEditor.rtti.h"
