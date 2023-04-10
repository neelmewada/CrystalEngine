#include "AssetsView.h"
#include "ui_AssetsView.h"

#include "System.h"

#include "AssetsViewFolderModel.h"
#include "AssetsViewContentModel.h"
#include "AssetViewItem.h"

#include "QtComponents.h"

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
        ui->assetsContentView->setContextMenuPolicy(::Qt::CustomContextMenu);
        
        // Content View Connections
        connect(ui->assetsContentView, &QTableView::customContextMenuRequested, this, &AssetsView::OnAssetViewItemContextMenuRequested);
        connect(ui->assetsContentView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &AssetsView::OnAssetSelectionChanged);
        connect(ui->assetsContentView, &QAbstractItemView::doubleClicked, this, &AssetsView::OnAssetViewItemDoubleClicked);

        ui->folderTreeView->selectionModel()->select(folderModel->index(0, 0), QItemSelectionModel::Select);
        UpdateContentView();

        auto signal = AssetManager::Type()->FindFunctionWithName("OnAssetDatabaseUpdated");
        auto event = Type()->FindFunctionWithName("OnAssetDatabaseUpdated");
        Object::Bind(&AssetManager::Get(), signal, this, event);
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
        currentDirectory = root;

        contentModel->SetDirectoryEntry(root);
        emit contentModel->modelReset({});
        
        int maxNumItemsInRow = Math::Max(1, ui->assetsContentView->width() / AssetsViewContentModel::sizePerItem);

        int totalItemCount = root->children.GetSize();

        int numCols = Math::Max(totalItemCount / maxNumItemsInRow, maxNumItemsInRow);
        int numRows = totalItemCount / numCols + 1;

        for (int c = 0; c < numCols; c++)
        {
            ui->assetsContentView->setColumnWidth(c, AssetsViewContentModel::sizePerItem);
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
            ui->selectedAssetLabel->setText(QString("%1 files selected").arg((int)selection.size()));
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

    void AssetsView::OnAssetViewItemContextMenuRequested(const QPoint& pos)
    {
        auto selection = ui->assetsContentView->selectionModel()->selectedIndexes();

        if (contextMenu == nullptr)
        {
            contextMenu = new Qt::ContextMenuWidget(this);
        }

        Array<AssetDatabaseEntry*> selected{};
        for (const auto& item : selection)
        {
            if (item.isValid() && item.internalPointer() != nullptr)
                selected.Add((AssetDatabaseEntry*)item.internalPointer());
        }

        contextMenu->Clear();

        if (selection.size() == 0)
        {
            PopulateCreateContextEntries(contextMenu);
        }
        else
        {
            contextMenu->AddCategoryLabel("Edit Options");
            contextMenu->AddRegularItem("Duplicate", ":/Editor/Icons/duplicate");
            contextMenu->AddRegularItem("Delete", ":/Editor/Icons/bin-white")
                ->BindMouseClickSignal(this, SLOT(OnContextMenuDeletePressed()));
        }

        auto moveToItem = contextMenu->AddRegularItem("Move To");
        moveToItem->SetSubmenuEnabled(true);
        moveToItem->GetSubmenu()->AddCategoryLabel("Move Options");
        moveToItem->GetSubmenu()->AddRegularItem("Within project")
            ->BindMouseClickSignal([]() -> void { CE_LOG(Info, All, "Within project!"); });
        moveToItem->GetSubmenu()->AddRegularItem("To another project");
        
        contextMenu->AddCategoryLabel("Asset Options");
        contextMenu->AddCategoryLabel("Misc Options");

        auto globalPos = ui->assetsContentView->mapToGlobal(pos);
        contextMenu->ShowPopup(globalPos);
    }

    void AssetsView::PopulateCreateContextEntries(Qt::ContextMenuWidget* contextMenu)
    {
        contextMenu->AddCategoryLabel("Create");
        contextMenu->AddRegularItem("New Folder", ":/Editor/Icons/add-folder")
            ->BindMouseClickSignal(this, SLOT(OnContextMenuNewFolderPressed()));
    }

    void AssetsView::OnContextMenuNewFolderPressed()
    {
        const auto& projectSettings = ProjectSettings::Get();
        auto gameAssetsDir = projectSettings.GetEditorProjectDirectory();

        if (!gameAssetsDir.Exists())
            return;

        auto newFolderPath = gameAssetsDir / currentDirectory->virtualPath / "New Folder";

        IO::Path::CreateDirectories(newFolderPath);
    }

    void AssetsView::OnContextMenuDeletePressed()
    {

    }

    void AssetsView::OnAssetDatabaseUpdated()
    {
        QMetaObject::invokeMethod(this, [=]{
            auto selection = ui->folderTreeView->selectionModel()->selectedIndexes();
            if (selection.size() > 0)
            {
                emit folderModel->modelReset({});
                ui->folderTreeView->selectionModel()->select(selection.at(0), QItemSelectionModel::Select);
            }
            UpdateContentView();
        });
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

