#ifndef SCENEEDITOR_H
#define SCENEEDITOR_H

#include <QMainWindow>

#include "DockManager.h"

namespace Ui {
class SceneEditorWindow;
}

namespace CE::Editor
{

    class SceneEditorWindow : public QMainWindow
    {
        Q_OBJECT

    public:
        explicit SceneEditorWindow(QWidget* parent = nullptr);
        ~SceneEditorWindow();

    private:
        Ui::SceneEditorWindow* ui;

        ads::CDockManager* dockManager = nullptr;
    };

}

#endif // SCENEEDITOR_H
