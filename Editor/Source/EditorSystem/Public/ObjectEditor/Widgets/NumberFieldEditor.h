#pragma once

namespace CE::Editor
{
    CLASS()
    class EDITORSYSTEM_API NumberFieldEditor : public FieldEditor
    {
        CE_CLASS(NumberFieldEditor, FieldEditor)
    public:

        void BindField(FieldType* field, void* instance) override;

    protected:

        void Construct() override;

        FIELD()
        CTextInput* textInput = nullptr;

    };
    
} // namespace CE::Editor

#include "NumberFieldEditor.rtti.h"