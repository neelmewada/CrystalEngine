#include "SceneOutlinerView.h"
#include "ui_SceneOutlinerView.h"

#include "GameFramework/GameFramework.h"

#include <QMenu>

namespace CE::Editor
{

    SceneOutlinerView::SceneOutlinerView(QWidget *parent)
        : EditorViewBase(parent)
        , ui(new Ui::SceneOutlinerView)
    {
        ui->setupUi(this);
        
        setWindowTitle("Scene Outliner");
        
        ui->treeWidget->setIndentation(8);

        this->setContextMenuPolicy(::Qt::CustomContextMenu);
        connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
            this, SLOT(ShowContextMenu(const QPoint&)));
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

    void SceneOutlinerView::Update()
    {
        ui->treeWidget->update();
    }

    void SceneOutlinerView::on_contextMenu_EmptyGameObject()
    {
        emit CreateEmptyGameObject();
    }
    
    void SceneOutlinerView::OnSceneOpened(Scene* scene)
    {
        if (model != nullptr && scene != nullptr)
        {
            model->OnSceneOpened(scene);
        }
    }

    void SceneOutlinerView::ShowContextMenu(const QPoint& pos)
    {
        QMenu contextMenu(tr("Context menu"), this);

        QAction action1("Empty GameObject", this);
        connect(&action1, SIGNAL(triggered()), this, SLOT(on_contextMenu_EmptyGameObject()));
        contextMenu.addAction(&action1);

        QAction action2("GameObject", this);
        contextMenu.addAction(&action2);

        contextMenu.exec(mapToGlobal(pos));
    }
}
