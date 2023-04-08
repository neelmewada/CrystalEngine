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
        
        // Folder View/Model
        folderModel = new AssetsViewFolderModel(this);
        ui->folderTreeView->setModel(folderModel);
        
        // Folder View Connections
        connect(ui->folderTreeView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &AssetsView::OnFolderSelectionChanged);
        
        // Content View/Model
        ui->assetsContentView->setAcceptDrops(true);
        contentModel = new AssetsViewContentModel(ui->assetsContentView, this);
        ui->assetsContentView->setModel(contentModel);
        ui->assetsContentView->setItemDelegate(new AssetsViewItemDelegate(this));
        
        // Content View Connections
        connect(ui->assetsContentView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &AssetsView::OnAssetSelectionChanged);
        connect(ui->assetsContentView, &QAbstractItemView::doubleClicked, this, &AssetsView::OnAssetViewItemDoubleClicked);
    }

    AssetsView::~AssetsView()
    {
        delete ui;
    }

    void AssetsView::resizeEvent(QResizeEvent* event)
    {
        EditorViewBase::resizeEvent(event);
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
        
        int maxNumItemsInRow = Math::Max(1, ui->assetsContentView->width() / AssetsViewContentModel::sizePerItem);

        int totalItemCount = root->children.GetSize();

        int numCols = Math::Max(totalItemCount / maxNumItemsInRow, maxNumItemsInRow);
        int numRows = totalItemCount / numCols + 1;

        for (int c = 0; c < numCols; c++)
        {
            ui->assetsContentView->setColumnWidth(c, 70);
        }

        for (int r = 0; r < numRows; r++)
        {
            ui->assetsContentView->setRowHeight(r, 86);
        }

        for (int i = 0; i < root->children.GetSize(); i++)
        {
            int c = i % numCols;
            int r = i / numCols;

            auto idx = contentModel->CreateIndex(r, c, root->children[i]);
            if (!ui->assetsContentView->isPersistentEditorOpen(idx))
            {
                ui->assetsContentView->openPersistentEditor(idx);
            }
        }
    }

    void AssetsView::OnAssetSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
    {
        auto selection = ui->assetsContentView->selectionModel()->selectedIndexes();

        if (selection.size() == 1 && selection.at(0).internalPointer() != nullptr)
        {
            auto entry = (AssetDatabaseEntry*)selection.at(0).internalPointer();
            ui->selectedAssetLabel->setText(QString(entry->name.GetCString()));
            return;
        }
        else if (selection.size() > 1)
        {
            ui->selectedAssetLabel->setText(QString("%1 files selected").arg(selection.size()));
            return;
        }

        ui->selectedAssetLabel->setText("");
    }

    void AssetsView::OnAssetViewItemDoubleClicked(const QModelIndex& index)
    {
        if (!index.isValid() || index.internalPointer() == nullptr)
            return;

        auto entry = (AssetDatabaseEntry*)index.internalPointer();

        if (entry->entryType == AssetDatabaseEntry::Type::Asset)
        {
            // TODO: Asset double clicked
            return;
        }

        auto selection = ui->folderTreeView->selectionModel()->selectedIndexes();
        if (selection.size() == 0 || selection.at(0).internalPointer() == nullptr)
        {
            contentModel->SetDirectoryEntry(nullptr);
            emit contentModel->modelReset({});
            return;
        }

        auto currentDirectory = (AssetDatabaseEntry*)selection.at(0).internalPointer();

        for (int i = 0; i < currentDirectory->children.GetSize(); i++)
        {
            if (currentDirectory->children[i] == entry)
            {
                ui->folderTreeView->selectionModel()->clearSelection();

                auto idx = folderModel->GetIndex(entry);
                ui->folderTreeView->selectionModel()->select(idx, QItemSelectionModel::Select);
                break;
            }
        }

        UpdateContentView();
    }

    void AssetsView::OnFolderSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
    {
        UpdateContentView();
    }

    AssetsViewItemDelegate::AssetsViewItemDelegate(QObject* parent)
        : QItemDelegate(parent)
    {

    }

    AssetsViewItemDelegate::~AssetsViewItemDelegate()
    {

    }

    QWidget* AssetsViewItemDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
    {
        if (index.isValid() && index.internalPointer() != nullptr)
        {
            auto entry = (AssetDatabaseEntry*)index.internalPointer();
            auto itemView = new AssetViewItem(parent);
            itemView->SetEntry(entry);
            return itemView;
        }
        return QItemDelegate::createEditor(parent, option, index);
    }

}

