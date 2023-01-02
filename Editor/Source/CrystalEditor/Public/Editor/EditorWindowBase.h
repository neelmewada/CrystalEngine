#pragma once

#include <QMainWindow>

#include "DockManager.h"

namespace CE
{
    class Scene;
}

namespace CE::Editor
{

    class CRYSTALEDITOR_API EditorWindowBase : public QMainWindow
    {
        Q_OBJECT
        
    public:
        explicit EditorWindowBase(QWidget* parent = nullptr);
        virtual ~EditorWindowBase();
        
    protected:
        Scene* editorScene = nullptr;
    };
    
} // namespace CE
