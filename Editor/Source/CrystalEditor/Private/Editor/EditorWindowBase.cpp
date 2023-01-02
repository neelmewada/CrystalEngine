
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
