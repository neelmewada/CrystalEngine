#pragma once

#include "FieldDrawer.h"

namespace CE::Editor
{

    class CRYSTALEDITOR_API VectorFieldDrawer : public FieldDrawer
    {
        CE_CLASS(VectorFieldDrawer, FieldDrawer)
    protected:
        VectorFieldDrawer();
    public:
        virtual ~VectorFieldDrawer();

        virtual void CreateGUI(QLayout* container);

    };
    
} // namespace CE::Editor

CE_RTTI_CLASS(CRYSTALEDITOR_API, CE::Editor, VectorFieldDrawer,
    CE_SUPER(CE::Editor::FieldDrawer),
    CE_NOT_ABSTRACT,
    CE_ATTRIBS(),
    CE_FIELD_LIST(),
    CE_FUNCTION_LIST()
)
