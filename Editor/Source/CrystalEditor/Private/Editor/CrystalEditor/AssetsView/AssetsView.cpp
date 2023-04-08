#include "AssetsView.h"
#include "ui_AssetsView.h"

#include "System.h"

#include "AssetsViewFolderModel.h"
#include "AssetsViewContentModel.h"
#include "AssetViewItem.h"

namespace CE::Editor
{

    AssetsView::AssetsView(QWidget *parent) :
        EditorViewBase(parent),
        ui(new Ui::AssetsView)
    {
        ui->setupUi(this);

        setWindowTitle("Assets");

        ui->splitter->setSizes({ 200, 500 });

        auto rootEntry = (AssetDatabaseEntry*)(AssetDatabase::Get().GetRootEntry());
        
        // Folder Model
        folderModel = new AssetsViewFolderModel(this);
        ui->folderTreeView->setModel(folderModel);
        
        connect(ui->folderTreeView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &AssetsView::OnFolderSelectionChanged);
        
        // Content Model
        contentModel = new AssetsViewContentModel(this);
        ui->assetsContentView->setModel(contentModel);
    }

    AssetsView::~AssetsView()
    {
        delete ui;
    }

    void AssetsView::resizeEvent(QResizeEvent* event)
    {
        EditorViewBase::resizeEvent(event);

        UpdateContentView();
    }

    void AssetsView::UpdateContentView()
    {
        auto selection = ui->folderTreeView->selectionModel()->selectedIndexes();
        if (selection.size() == 0)
        {
            contentModel->SetDirectoryEntry(nullptr);
            emit contentModel->modelReset({});
            return;
        }

        const auto& index = selection.at(0);

        if (!index.isValid() || index.internalPointer() == nullptr)
        {
            contentModel->SetDirectoryEntry(nullptr);
            emit contentModel->modelReset({});
            return;
        }

        auto root = (AssetDatabaseEntry*)index.internalPointer();

        contentModel->SetDirectoryEntry(root);
        emit contentModel->modelReset({});
    }

    void AssetsView::OnFolderSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
    {
        UpdateContentView();
    }

}

