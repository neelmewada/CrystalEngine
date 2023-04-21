#ifndef CRYSTALEDITORWINDOW_H
#define CRYSTALEDITORWINDOW_H

#include "CoreMinimal.h"
#include "System.h"

#include "Editor/CrystalEditorBus.h"
#include "Editor/EditorWindow.h"

#include <QWindow>
#include <QMainWindow>

#include "DockManager.h"

namespace Ui {
class CrystalEditorWindow;
}

namespace ads
{
    class CDockWidget;
}

namespace CE::Editor
{
    class SceneEditorWindow;
    class AssetImporterWindow;

    class EditorWindow;

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

        FUNCTION(Event)
        void BrowseToAsset(AssetDatabaseEntry* assetEntry) override;

        EditorWindow* GetEditorWindow(ClassType* editorWindowType);

        ads::CDockWidget* GetEditorWindowDockWidget(ClassType* editorWindowType);

        // ******************************************
        // Public API

        template<typename TEditorWindowClass>
        TEditorWindowClass* GetEditorWindow()
        {
            return (TEditorWindowClass*)GetEditorWindow(TEditorWindowClass::Type());
        }

        template<typename TEditorWindowClass>
        ads::CDockWidget* GetEditorWindowDockWidget()
        {
            return GetEditorWindowDockWidget(TEditorWindowClass::Type());
        }

    private:

        void CreateSceneEditorWindow();

    private slots:
        void on_actionExit_triggered();

        void on_actionOpen_triggered();

        void on_actionAssetImporter_triggered();

    private:
        Ui::CrystalEditorWindow* ui;

        ads::CDockManager* mainDockManager = nullptr;
        AssetImporterWindow* assetImporterWindow = nullptr;

        Array<ads::CDockWidget*> dockWidgets{};
        Vector<EditorWindow*> editorWindows{};
    };
}

#include "CrystalEditorWindow.rtti.h"

#endif // CRYSTALEDITORWINDOW_H
