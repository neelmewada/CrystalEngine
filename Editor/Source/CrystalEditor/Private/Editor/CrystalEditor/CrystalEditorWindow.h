#ifndef CRYSTALEDITORWINDOW_H
#define CRYSTALEDITORWINDOW_H

#include "CoreMinimal.h"
#include "System.h"

#include "Editor/CrystalEditorBus.h"

#include <QWindow>
#include <QMainWindow>

#include "DockManager.h"

namespace Ui {
class CrystalEditorWindow;
}

namespace CE::Editor
{
    class SceneEditorWindow;

    CLASS()
    class CrystalEditorWindow : public QMainWindow, public CE::Object, public CrystalEditorBus::Interface
    {
        Q_OBJECT
        CE_CLASS(CrystalEditorWindow, CE::Object)
    public:
        explicit CrystalEditorWindow(QWidget* parent = nullptr);
        ~CrystalEditorWindow();

        void LoadProject(IO::Path projectPath);

        FUNCTION(Event)
        void OnAssetUpdated(IO::Path assetPath);

        // *****************************************
        // CrystalEditorBus::Interface

        FUNCTION(Event)
        void OpenAsset(AssetDatabaseEntry* assetEntry) override;

    private slots:
        void on_actionExit_triggered();

        void on_actionOpen_triggered();

    private:
        Ui::CrystalEditorWindow* ui;

        ads::CDockManager* mainDockManager = nullptr;

        Array<ads::CDockWidget*> dockWidgets{};
        Vector<EditorWindowBase*> editorWindows{};
    };
}

#include "CrystalEditorWindow.rtti.h"



#endif // CRYSTALEDITORWINDOW_H
