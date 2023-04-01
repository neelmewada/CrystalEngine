#include "AssetsView.h"
#include "ui_AssetsView.h"

#include "System.h"

#include "AssetsViewFolderModel.h"
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
    }

    AssetsView::~AssetsView()
    {
        delete ui;
    }

    void AssetsView::resizeEvent(QResizeEvent* event)
    {
        EditorViewBase::resizeEvent(event);

        auto selection = ui->folderTreeView->selectionModel()->selectedIndexes();
        if (selection.size() == 0)
            return;

        const auto& index = selection.at(0);

        if (!index.isValid() || index.internalPointer() == nullptr)
            return;

        auto root = (AssetDatabaseEntry*)index.internalPointer();

        ui->assetsGridView->ClearWidgets();

        for (auto& entry : root->children)
        {
            auto item = new AssetViewItem(this);
            item->SetEntry(&entry);
            ui->assetsGridView->AddWidget(item);
        }

        ui->assetsGridView->Update();
    }

    void AssetsView::OnFolderSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
    {
        auto selection = ui->folderTreeView->selectionModel()->selectedIndexes();
        if (selection.size() == 0)
            return;

        const auto& index = selection.at(0);
        
        if (!index.isValid() || index.internalPointer() == nullptr)
            return;

        auto root = (AssetDatabaseEntry*)index.internalPointer();

        ui->assetsGridView->ClearWidgets();
        
        for (auto& entry : root->children)
        {
            auto item = new AssetViewItem(this);
            item->SetEntry(&entry);
            ui->assetsGridView->AddWidget(item);
        }
        
        ui->assetsGridView->Update();
    }

}

