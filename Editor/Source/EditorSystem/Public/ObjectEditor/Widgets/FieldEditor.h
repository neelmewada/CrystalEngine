#pragma once

namespace CE::Editor
{
    CLASS(Abstract)
    class EDITORSYSTEM_API FieldEditor : public CWidget
    {
        CE_CLASS(FieldEditor, CWidget)
    public:

        virtual ~FieldEditor();

    protected:

        void Construct() override;

        FieldEditor();

        
    };

} // namespace CE::Editor

#include "FieldEditor.rtti.h"