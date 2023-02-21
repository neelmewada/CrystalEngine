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
        
        ui->treeWidget->setIndentation(12);

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

    QTreeView* SceneOutlinerView::GetTreeView() const
    {
        return ui->treeWidget;
    }

    void SceneOutlinerView::Refresh()
    {
        this->model->modelReset({});
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

        auto font = contextMenu.font();
        font.setPointSize(12);
        contextMenu.setFont(font);

        QAction action1("Empty GameObject", this);
        connect(&action1, SIGNAL(triggered()), this, SLOT(on_contextMenu_EmptyGameObject()));
        contextMenu.addAction(&action1);

        contextMenu.exec(mapToGlobal(pos));
    }
}

CE_RTTI_CLASS_IMPL(CRYSTALEDITOR_API, CE::Editor, SceneOutlinerView)
