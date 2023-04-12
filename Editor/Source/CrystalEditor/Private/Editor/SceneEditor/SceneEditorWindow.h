#ifndef SCENEEDITOR_H
#define SCENEEDITOR_H

#include <QMainWindow>

#include "Editor/EditorWindowBase.h"
#include "Editor/SceneEditor/SceneEditorBus.h"

namespace Ui {
class SceneEditorWindow;
}

namespace CE::Editor
{
    class SceneOutlinerView;
    class ViewportView;
    class DetailsView;
    class ConsoleView;
    class AssetsView;

    CLASS()
    class SceneEditorWindow
        : public EditorWindowBase
        , public SceneEditorBus::Interface
    {
        Q_OBJECT
        CE_CLASS(SceneEditorWindow, CE::Editor::EditorWindowBase)
    public:
        explicit SceneEditorWindow(QWidget* parent = nullptr);
        ~SceneEditorWindow();

    public:
        // Public API

        void OnSceneLoaded();

        // EditorWindowBase overrides

        virtual bool CanOpenAsset(AssetDatabaseEntry* assetEntry) override;

        virtual bool OpenAsset(AssetDatabaseEntry* assetEntry) override;

        // *****************************************
        // SceneEditorBus::Interface

        FUNCTION(Event)
        virtual void OpenEmptyScene() override;

        FUNCTION(Event)
        virtual void CreateEmptyGameObject() override;

        FUNCTION(Event)
        virtual void OpenScene(String sceneAssetPath) override;
        
    private slots:
        void OnLogPushedToConsole(const String& string);

        void on_createEmptyGameObject_triggered();

        // *****************************************
        // File menu actions

        void on_actionNewScene_triggered();

        void on_actionSaveScene_triggered();

        void on_actionSaveSceneAs_triggered();

        void on_actionOpenScene_triggered();

        void on_actionExit_triggered();

        // *****************************************
        // View menu actions

        void on_actionConsole_triggered();

        void on_actionSceneOutliner_triggered();

        void on_actionDetails_triggered();

        void on_actionViewport_triggered();

    private:
        Ui::SceneEditorWindow* ui;
        
        ads::CDockManager* dockManager = nullptr;
        
        String scenePath{};

        // Sub Views
        SceneOutlinerView* sceneOutlinerView = nullptr;
        ads::CDockWidget* sceneOutlinerViewDockWidget = nullptr;

        ViewportView* viewportView = nullptr;
        ads::CDockWidget* viewportViewDockWidget = nullptr;

        DetailsView* detailsView = nullptr;
        ads::CDockWidget* detailsViewDockWidget = nullptr;

        ConsoleView* consoleView = nullptr;
        ads::CDockWidget* consoleViewDockWidget = nullptr;

        AssetsView* assetsView = nullptr;
        ads::CDockWidget* assetsViewDockWidget = nullptr;
    };

}

#include "SceneEditorWindow.rtti.h"


#endif // SCENEEDITOR_H


