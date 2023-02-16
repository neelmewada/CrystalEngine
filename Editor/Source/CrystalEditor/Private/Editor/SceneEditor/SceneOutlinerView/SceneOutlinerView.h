#ifndef SCENEOUTLINERVIEW_H
#define SCENEOUTLINERVIEW_H

#include <QWidget>

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

    public:
        explicit SceneOutlinerView(QWidget *parent = nullptr);
        ~SceneOutlinerView();
        
        virtual void OnSceneOpened(Scene* scene) override;
        
        void SetModel(SceneOutlinerModel* model);
        
        CE_INLINE SceneOutlinerModel* GetModel() const { return model; }

        void Update();

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

#endif // SCENEOUTLINERVIEW_H
