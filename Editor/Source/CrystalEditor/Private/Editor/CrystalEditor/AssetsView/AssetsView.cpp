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

        UpdateContentView();
    }

    void AssetsView::UpdateContentView()
    {
        auto selection = ui->folderTreeView->selectionModel()->selectedIndexes();
        if (selection.size() == 0)
            return;

        const auto& index = selection.at(0);

        if (!index.isValid() || index.internalPointer() == nullptr)
            return;

        auto root = (AssetDatabaseEntry*)index.internalPointer();

        while (assetItems.GetSize() < root->children.GetSize())
        {
            auto item = new AssetViewItem(this);
            assetItems.Add(item);
        }

        int itemCount = root->children.GetSize();
        const int itemWidth = 80;

        int numCols = Math::Max(1, width() / itemWidth);
        int numRows = itemCount / numCols + 1;

        ui->assetsContentWidget->clear();

        for (int i = 0; i < itemCount; i++)
        {
            int r = i / numRows;
            int c = i % numCols;
            ui->assetsContentWidget->setItem(r, c, assetItems[i]);
            assetItems[i]->SetEntry(&root->children[i]);
        }
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

        
    }

}

