#ifndef CRYSTALEDITORWINDOW_H
#define CRYSTALEDITORWINDOW_H

#include "IO/Path.h"

#include <QMainWindow>

#include "DockManager.h"

namespace Ui {
class CrystalEditorWindow;
}

namespace CE::Editor
{
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
    };
}

#endif // CRYSTALEDITORWINDOW_H
