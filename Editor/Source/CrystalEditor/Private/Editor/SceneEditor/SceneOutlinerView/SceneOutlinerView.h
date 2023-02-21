#ifndef SCENEOUTLINERVIEW_H
#define SCENEOUTLINERVIEW_H

#include <QWidget>
#include <QTreeView>

#include "Editor/EditorViewBase.h"
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
        public EditorViewBase,
        public SceneOutlinerViewBus::Handler
    {
        Q_OBJECT

        CE_CLASS(SceneOutlinerView, EditorViewBase)

    public:
        explicit SceneOutlinerView(QWidget *parent = nullptr);
        ~SceneOutlinerView();
        
        virtual void OnSceneOpened(Scene* scene) override;
        
        void SetModel(SceneOutlinerModel* model);
        
        CE_INLINE SceneOutlinerModel* GetModel() const { return model; }

        QTreeView* GetTreeView() const;

        void Refresh();

    private slots:
        void ShowContextMenu(const QPoint& pos);

        void on_contextMenu_EmptyGameObject();

    signals: // Signals to Scene Editor Window
        void CreateEmptyGameObject();
        
    private:
        SceneOutlinerModel* model = nullptr;
        
        Ui::SceneOutlinerView *ui;
    };

}

CE_RTTI_CLASS(CRYSTALEDITOR_API, CE::Editor, SceneOutlinerView,
    CE_SUPER(CE::Editor::EditorViewBase),
    CE_DONT_INSTANTIATE,
    CE_ATTRIBS(),
    CE_FIELD_LIST(),
    CE_FUNCTION_LIST()
)

#endif // SCENEOUTLINERVIEW_H

