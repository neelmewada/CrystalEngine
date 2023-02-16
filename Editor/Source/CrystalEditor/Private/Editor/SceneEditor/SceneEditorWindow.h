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

    class SceneEditorWindow
        : public EditorWindowBase
        , public SceneEditorBus::Handler
    {
        Q_OBJECT

    public:
        explicit SceneEditorWindow(QWidget* parent = nullptr);
        ~SceneEditorWindow();

        virtual void OpenEmptyScene() override;

    private slots:
        void CreateEmptyGameObject();
        
    private slots:
        void on_actionNewScene_triggered();

        void on_actionSaveScene_triggered();

        void on_actionSaveSceneAs_triggered();

        void on_actionOpenScene_triggered();

    private:
        Ui::SceneEditorWindow* ui;
        
        ads::CDockManager* dockManager = nullptr;
        
        String scenePath{};
        SceneOutlinerView* sceneOutlinerView = nullptr;
        ViewportView* viewportView = nullptr;
    };

}

#endif // SCENEEDITOR_H
