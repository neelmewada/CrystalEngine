
#include "Editor/EditorWindow.h"

#include "GameFramework/Scene.h"

namespace CE::Editor
{

    EditorWindow::EditorWindow(QWidget* parent) : QMainWindow(parent)
    {
        editorScene = new Scene();
    }

    EditorWindow::~EditorWindow()
    {
        delete editorScene;
    }

}

