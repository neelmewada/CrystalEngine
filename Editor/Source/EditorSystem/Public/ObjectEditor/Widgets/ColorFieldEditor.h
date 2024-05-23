#pragma once

namespace CE::Editor
{
    CLASS()
    class EDITORSYSTEM_API ColorFieldEditor : public FieldEditor
    {
        CE_CLASS(ColorFieldEditor, FieldEditor)
    public:

        void BindField(FieldType* field, void* instance) override;

    protected:

        void Construct() override;


    };
    
} // namespace CE::Editor

#include "ColorFieldEditor.rtti.h"