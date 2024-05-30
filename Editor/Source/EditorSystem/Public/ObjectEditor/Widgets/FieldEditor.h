#pragma once

namespace CE::Editor
{
    CLASS(Abstract)
    class EDITORSYSTEM_API FieldEditor : public CWidget
    {
        CE_CLASS(FieldEditor, CWidget)
    public:

        virtual ~FieldEditor();

        virtual void BindField(FieldType* field, void* instance) = 0;

        // - Signals -

        CE_SIGNAL(OnValueUpdated);

    protected:

        FieldEditor();

        void Construct() override;


    };

} // namespace CE::Editor

#include "FieldEditor.rtti.h"