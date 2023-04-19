#ifndef SCENEOUTLINERVIEW_H
#define SCENEOUTLINERVIEW_H

#include <QWidget>
#include <QTreeView>

#include "Editor/EditorView.h"
#include "Editor/SceneEditor/SceneOutlinerViewBus.h"

#include "SceneOutlinerModel.h"

namespace Ui {
class SceneOutlinerView;
}

namespace CE
{
    class Scene;
}

namespace CE::Editor
{
    class SceneOutlinerView :
        public EditorView,
        public SceneOutlinerViewBus::Interface
    {
        Q_OBJECT

        CE_CLASS(SceneOutlinerView, EditorView)

    public:
        explicit SceneOutlinerView(QWidget *parent = nullptr);
        ~SceneOutlinerView();
        
        void SetModel(SceneOutlinerModel* model);
        
        CE_INLINE SceneOutlinerModel* GetModel() const { return model; }

        QTreeView* GetTreeView() const;

        void Refresh();

        void ClearSelection();

        // ************************************************
        // SceneOutlinerViewBus::Interface

        virtual void OnSceneOpened(Scene* scene) override;

    private slots:
        void ShowContextMenu(const QPoint& pos);

        void on_contextMenu_EmptyGameObject();

        void selectionChanged(const QItemSelection& selected, const QItemSelection& deselected);

    public slots:
        void UpdateSelectedGameObjectEntries(Array<GameObject*> gameObjects);

    signals: // Signals to Scene Editor Window
        void CreateEmptyGameObject();

        void OnSceneSelectionChanged(Array<GameObject*> selected);
        
    private:
        SceneOutlinerModel* model = nullptr;
        
        Ui::SceneOutlinerView *ui;
    };

}

CE_RTTI_CLASS(CRYSTALEDITOR_API, CE::Editor, SceneOutlinerView,
    CE_SUPER(CE::Editor::EditorView),
    CE_DONT_INSTANTIATE,
    CE_ATTRIBS(),
    CE_FIELD_LIST(),
    CE_FUNCTION_LIST(
        // SceneOutlinerViewBus::Interface
        CE_FUNCTION(OnSceneOpened, Event)
    )
)

#endif // SCENEOUTLINERVIEW_H

