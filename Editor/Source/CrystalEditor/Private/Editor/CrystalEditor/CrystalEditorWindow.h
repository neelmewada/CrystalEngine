#ifndef CRYSTALEDITORWINDOW_H
#define CRYSTALEDITORWINDOW_H

#include "CoreMinimal.h"

#include <QWindow>
#include <QMainWindow>

#include "DockManager.h"

namespace Ui {
class CrystalEditorWindow;
}

namespace CE::Editor
{
    class SceneEditorWindow;

    class CrystalEditorWindow : public QMainWindow
    {
        Q_OBJECT

    public:
        explicit CrystalEditorWindow(QWidget* parent = nullptr);
        ~CrystalEditorWindow();

        void LoadProject(IO::Path projectPath);

    private:
        Ui::CrystalEditorWindow* ui;

        ads::CDockManager* mainDockManager = nullptr;

        Array<ads::CDockWidget*> dockWidgets{};
    };
}

#endif // CRYSTALEDITORWINDOW_H
