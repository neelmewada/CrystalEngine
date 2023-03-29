#include "AssetsView.h"
#include "ui_AssetsView.h"

#include "AssetsViewFolderModel.h"
#include "AssetsViewContentModel.h"

#include "System.h"

namespace CE::Editor
{

    AssetsView::AssetsView(QWidget *parent) :
        EditorViewBase(parent),
        ui(new Ui::AssetsView)
    {
        ui->setupUi(this);

        setWindowTitle("Assets");

        ui->splitter->setSizes({ 200, 500 });
        
        // Folder Model
        folderModel = new AssetsViewFolderModel(this);
        ui->folderTreeView->setModel(folderModel);
        
        connect(ui->folderTreeView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &AssetsView::OnFolderSelectionChanged);

        // Content Model
        contentModel = new AssetsViewContentModel(this);
        ui->assetsTableView->setModel(contentModel);
    }

    AssetsView::~AssetsView()
    {
        delete ui;
    }

    void AssetsView::OnFolderSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
    {
        auto selection = ui->folderTreeView->selectionModel()->selectedIndexes();
        if (selection.size() == 0)
            return;

        const auto& index = selection.at(0);
        
        if (!index.isValid() || index.internalPointer() == nullptr)
            return;

        auto entry = (AssetDatabaseEntry*)index.internalPointer();
        
        auto search = AssetDatabase::Get().GetEntry(entry->virtualPath);

    }

}

