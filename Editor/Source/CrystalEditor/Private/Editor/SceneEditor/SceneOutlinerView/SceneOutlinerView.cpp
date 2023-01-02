#include "SceneOutlinerView.h"
#include "ui_SceneOutlinerView.h"

#include "GameFramework/GameFramework.h"

namespace CE::Editor
{

    SceneOutlinerView::SceneOutlinerView(QWidget *parent)
        : EditorViewBase(parent)
        , ui(new Ui::SceneOutlinerView)
    {
        ui->setupUi(this);
        
        setWindowTitle("Scene Outliner");
        
        ui->treeWidget->setIndentation(8);
    }

    SceneOutlinerView::~SceneOutlinerView()
    {
        delete ui;
    }

    void SceneOutlinerView::SetModel(SceneOutlinerModel* model)
    {
        this->model = model;
        ui->treeWidget->QTreeView::setModel(model);
    }
    
    void SceneOutlinerView::OnSceneOpened(Scene* scene)
    {
        if (model != nullptr && scene != nullptr)
        {
            model->OnSceneOpened(scene);
        }
    }

}
