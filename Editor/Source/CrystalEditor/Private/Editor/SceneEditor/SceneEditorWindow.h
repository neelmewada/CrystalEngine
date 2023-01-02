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
        
    private:
        Ui::SceneEditorWindow* ui;
        
        ads::CDockManager* dockManager = nullptr;
        
        SceneOutlinerView* sceneOutlinerView = nullptr;
        ViewportView* viewportView = nullptr;
    };

}

#endif // SCENEEDITOR_H
