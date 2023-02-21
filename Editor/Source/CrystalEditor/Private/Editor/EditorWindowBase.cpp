
#include "Editor/EditorWindowBase.h"

#include "GameFramework/Scene.h"

namespace CE::Editor
{

    EditorWindowBase::EditorWindowBase(QWidget* parent) : QMainWindow(parent)
    {
        editorScene = new Scene();
    }

    EditorWindowBase::~EditorWindowBase()
    {
        delete editorScene;
    }

}

CE_RTTI_CLASS_IMPL(CRYSTALEDITOR_API, CE::Editor, EditorWindowBase)
