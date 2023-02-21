#pragma once

#include "Object/Object.h"

#include <QWidget>

namespace CE::Editor
{

    class EditorViewBase : public QWidget, public CE::Object
    {
        Q_OBJECT

        CE_CLASS(EditorViewBase, CE::Object)
        
    public:
        explicit EditorViewBase(QWidget* parent = nullptr);
        virtual ~EditorViewBase();
        
    };
    
} // namespace CE::Editor

CE_RTTI_CLASS(CRYSTALEDITOR_API, CE::Editor, EditorViewBase,
    CE_SUPER(CE::Object),
    CE_DONT_INSTANTIATE,
    CE_ATTRIBS(),
    CE_FIELD_LIST(),
    CE_FUNCTION_LIST()
)
