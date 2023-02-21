#pragma once

#include <QMainWindow>

#include "DockManager.h"

namespace CE
{
    class Scene;
}

namespace CE::Editor
{

    class CRYSTALEDITOR_API EditorWindowBase : public QMainWindow, public CE::Object
    {
        Q_OBJECT

        CE_CLASS(EditorWindowBase, CE::Object)
        
    public:
        explicit EditorWindowBase(QWidget* parent = nullptr);
        virtual ~EditorWindowBase();
        
    protected:
        Scene* editorScene = nullptr;
    };
    
} // namespace CE

CE_RTTI_CLASS(CRYSTALEDITOR_API, CE::Editor, EditorWindowBase,
    CE_SUPER(CE::Object),
    CE_DONT_INSTANTIATE,
    CE_ATTRIBS(),
    CE_FIELD_LIST(),
    CE_FUNCTION_LIST(
        
    )
)
