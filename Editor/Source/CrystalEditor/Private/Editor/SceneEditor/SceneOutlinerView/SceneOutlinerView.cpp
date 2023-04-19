#include "SceneOutlinerView.h"
#include "ui_SceneOutlinerView.h"

#include "GameFramework/GameFramework.h"

#include "SceneOutlinerViewSelectionModel.h"

#include <QMenu>

namespace CE::Editor
{

    SceneOutlinerView::SceneOutlinerView(QWidget *parent)
        : EditorView(parent)
        , ui(new Ui::SceneOutlinerView)
    {
        ui->setupUi(this);

        CE_CONNECT(SceneOutlinerViewBus, this);
        
        setWindowTitle("Scene Outliner");
        
        ui->treeWidget->setIndentation(12);

        this->setContextMenuPolicy(::Qt::CustomContextMenu);

        connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
            this, SLOT(ShowContextMenu(const QPoint&)));
    }

    SceneOutlinerView::~SceneOutlinerView()
    {
        CE_DISCONNECT(SceneOutlinerViewBus, this);

        delete ui;
    }

    void SceneOutlinerView::SetModel(SceneOutlinerModel* model)
    {
        this->model = model;
        ui->treeWidget->QTreeView::setModel(model);

        QObject::connect(ui->treeWidget->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &SceneOutlinerView::selectionChanged);
    }

    QTreeView* SceneOutlinerView::GetTreeView() const
    {
        return ui->treeWidget;
    }

    void SceneOutlinerView::Refresh()
    {
        this->model->modelReset({});
    }

    void SceneOutlinerView::ClearSelection()
    {
        ui->treeWidget->clearSelection();
    }

    void SceneOutlinerView::on_contextMenu_EmptyGameObject()
    {
        emit CreateEmptyGameObject();
    }

    void SceneOutlinerView::selectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
    {
        auto selectedList = ui->treeWidget->selectionModel()->selectedIndexes();
        
        Array<GameObject*> selectedGO{};

        for (const auto& selection : selectedList)
        {
            if (selection.internalPointer() == nullptr)
                continue;
            selectedGO.Add((GameObject*)selection.internalPointer());
        }

        emit OnSceneSelectionChanged(selectedGO);
    }

    void SceneOutlinerView::UpdateSelectedGameObjectEntries(Array<GameObject*> gameObjects)
    {
        ui->treeWidget->dataChanged(QModelIndex(), QModelIndex());
    }
    
    void SceneOutlinerView::OnSceneOpened(Scene* scene)
    {
        if (model != nullptr)
        {
            model->OnSceneOpened(scene);
        }
    }

    void SceneOutlinerView::ShowContextMenu(const QPoint& pos)
    {
        if (model->GetScene() == nullptr)
            return;

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
