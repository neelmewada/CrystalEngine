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

    class SceneEditorWindow
        : public EditorWindowBase
        , public SceneEditorBus::Interface
    {
        Q_OBJECT
        CE_CLASS(SceneEditorWindow, EditorWindowBase)
    public:
        explicit SceneEditorWindow(QWidget* parent = nullptr);
        ~SceneEditorWindow();

        ///////////////////////////////////////
        // SceneEditorBus::Interface

        virtual void OpenEmptyScene() override;

        virtual void CreateEmptyGameObject() override;

        virtual void OpenScene(String sceneAssetPath) override;
        
    private slots:
        void on_createEmptyGameObject_triggered();

        void on_actionNewScene_triggered();

        void on_actionSaveScene_triggered();

        void on_actionSaveSceneAs_triggered();

        void on_actionOpenScene_triggered();

        // *****************************************
        // Menu actions

        void on_actionConsole_triggered();

    private:
        Ui::SceneEditorWindow* ui;
        
        ads::CDockManager* dockManager = nullptr;
        
        String scenePath{};

        // Sub Views
        SceneOutlinerView* sceneOutlinerView = nullptr;
        ViewportView* viewportView = nullptr;
        DetailsView* detailsView = nullptr;
        ConsoleView* consoleView = nullptr;
        ads::CDockWidget* consoleViewDockWidget = nullptr;
    };

}

CE_RTTI_CLASS(CRYSTALEDITOR_API, CE::Editor, SceneEditorWindow,
    CE_SUPER(CE::Editor::EditorWindowBase),
    CE_DONT_INSTANTIATE,
    CE_ATTRIBS(),
    CE_FIELD_LIST(),
    CE_FUNCTION_LIST(
        // SceneEditorBus::Interface
        CE_FUNCTION(OpenEmptyScene, Event)
        CE_FUNCTION(CreateEmptyGameObject, Event)
        CE_FUNCTION(OpenScene, Event)
    )
)

#endif // SCENEEDITOR_H


