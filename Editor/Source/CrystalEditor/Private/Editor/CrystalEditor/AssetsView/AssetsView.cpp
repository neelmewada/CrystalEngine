#include "AssetsView.h"
#include "ui_AssetsView.h"

#include "CoreMinimal.h"
#include "System.h"

#include "AssetsViewFolderModel.h"
#include "AssetsViewContentModel.h"
#include "AssetViewItem.h"

#include "QtComponents.h"

#include "AssetProcessor.h"

#include <QFileDialog>
#include <QDesktopServices>

#include <QProcessEnvironment>

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
        itemDelegate = new AssetsViewItemDelegate(this);
        ui->assetsContentView->setItemDelegate(itemDelegate);
        ui->assetsContentView->setContextMenuPolicy(::Qt::CustomContextMenu);
        
        // Content View Connections
        connect(ui->assetsContentView, &QTableView::customContextMenuRequested, this, &AssetsView::OnAssetViewItemContextMenuRequested);
        connect(ui->assetsContentView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &AssetsView::OnAssetSelectionChanged);
        connect(ui->assetsContentView, &QAbstractItemView::doubleClicked, this, &AssetsView::OnAssetViewItemDoubleClicked);

        ui->folderTreeView->selectionModel()->select(folderModel->index(0, 0), QItemSelectionModel::Select);
        UpdateContentView();

        auto signal = AssetManager::Type()->FindFunctionWithName("OnAssetDatabaseUpdated");
        auto event = Self::Type()->FindFunctionWithName("OnAssetDatabaseUpdated");
        if (signal != nullptr && event != nullptr)
            Object::Bind(&AssetManager::Get(), signal, this, event);

        signal = AssetManager::Type()->FindFunctionWithName("OnNewSourceAssetAdded");
        event = Self::Type()->FindFunctionWithName("OnNewSourceAssetAdded");
        if (signal != nullptr && event != nullptr)
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

    void AssetsView::SelectAsset(AssetDatabaseEntry* assetEntry)
    {
        if (assetEntry == nullptr || assetEntry->parent == nullptr)
            return;

        auto index = folderModel->GetIndex(assetEntry->parent);
        if (!index.isValid())
            return;

        ui->folderTreeView->selectionModel()->clearSelection();
        ui->folderTreeView->selectionModel()->select(index, QItemSelectionModel::Select);
        ui->assetsContentView->selectionModel()->clearSelection();

        int rowCount = contentModel->rowCount();
        int colCount = contentModel->columnCount();

        for (int r = 0; r < rowCount; r++)
        {
            for (int c = 0; c < colCount; c++)
            {
                int i = r * colCount + c;
                auto idx = contentModel->index(r, c);
                if (!idx.isValid())
                    continue;
                auto entry = (AssetDatabaseEntry*)idx.internalPointer();
                if (entry == nullptr)
                    continue;
                if (entry == assetEntry)
                {
                    ui->assetsContentView->selectionModel()->select(idx, QItemSelectionModel::Select);
                    return;
                }
            }
        }
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
            CE_PUBLISH(CrystalEditorBus, OpenAsset, entry);
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
        QModelIndexList selection = ui->assetsContentView->selectionModel()->selectedIndexes();

        const auto& projectSettings = ProjectSettings::Get();
        auto projectPath = projectSettings.GetEditorProjectDirectory();

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
            bool canEdit = true;
            for (const QModelIndex& idx : selection)
            {
                if (idx.internalPointer() != nullptr)
                {
                    AssetDatabaseEntry* entry = (AssetDatabaseEntry*)idx.internalPointer();
                    if (entry->category != AssetDatabaseEntry::Category::GameAssets &&
                        entry->category != AssetDatabaseEntry::Category::GameShaders)
                    {
                        canEdit = false;
                        break;
                    }
                }
            }
            if (canEdit)
            {
                contextMenu->AddCategoryLabel("Edit Options");
                if (selection.size() == 1)
                {
                    contextMenu->AddRegularItem("Rename", ":/Editor/Icons/rename")
                        ->BindMouseClickSignal(this, SLOT(OnContextMenuRenamePressed()));
                }
                contextMenu->AddRegularItem("Duplicate", ":/Editor/Icons/duplicate")
                    ->BindMouseClickSignal(this, SLOT(OnContextMenuDuplicatePressed()));
                contextMenu->AddRegularItem("Move To", ":/Editor/Icons/move")
                    ->BindMouseClickSignal(this, SLOT(OnContextMenuMoveToPressed()));
                contextMenu->AddRegularItem("Delete", ":/Editor/Icons/bin-white")
                    ->BindMouseClickSignal(this, SLOT(OnContextMenuDeletePressed()));
            }
        }

        bool canShowAssetOptions = selected.GetSize() > 0;
        BuiltinAssetType builtinAssetType = BuiltinAssetType::None;
        ClassType* customAssetClass = nullptr;

        for (auto entry : selected)
        {
            if (entry->entryType == AssetDatabaseEntry::Type::Directory)
            {
                canShowAssetOptions = false;
                break;
            }
            if (entry->category != AssetDatabaseEntry::Category::GameAssets &&
                entry->category != AssetDatabaseEntry::Category::GameShaders)
            {
                canShowAssetOptions = false;
                break;
            }

            auto curBuiltinAssetType = Asset::GetBuiltinAssetTypeFor(entry->extension);
            if (curBuiltinAssetType != BuiltinAssetType::None)
            {
                if (customAssetClass != nullptr)
                {
                    canShowAssetOptions = false;
                    break;
                }
                if (builtinAssetType != BuiltinAssetType::None && builtinAssetType != curBuiltinAssetType)
                {
                    canShowAssetOptions = false;
                    break;
                }

                builtinAssetType = curBuiltinAssetType;
            }

            auto curCustomAssetClass = Asset::GetAssetClassFor(entry->extension);
            if (curCustomAssetClass != nullptr)
            {
                if (builtinAssetType != BuiltinAssetType::None)
                {
                    canShowAssetOptions = false;
                    break;
                }
                if (customAssetClass != nullptr && customAssetClass != curCustomAssetClass)
                {
                    canShowAssetOptions = false;
                    break;
                }

                customAssetClass = curCustomAssetClass;
            }
        }

        if (selected.GetSize() > 0)
        {
            contextMenu->AddCategoryLabel("Asset Options");
            if (canShowAssetOptions && customAssetClass != nullptr)
            {
                contextMenu->AddRegularItem("Reimport", ":/Editor/Icons/download")
                        ->BindMouseClickSignal([this,selected]
                                               {
                                                   this->ReimportAssets(selected);
                                               });
            }

            auto assetPath = projectPath / selected.At(0)->GetVirtualPath();
            auto platformNameLowercase = PlatformMisc::GetPlatformName().ToLower();
            contextMenu->AddRegularItem(QString("Show in %1").arg(PlatformMisc::GetSystemFileExplorerDisplayName().GetCString()),
                                        QString(":/Editor/Icons/explorer-%1").arg(platformNameLowercase.GetCString()))
                ->BindMouseClickSignal([this,assetPath]()
                                       {
                                            if (!assetPath.Exists())
                                                return;
                                            auto assetPathDir = assetPath.GetString();
                                            EditorPlatformUtils::RevealInFileExplorer(QString(assetPathDir.GetCString()));
                                       });
        }

        auto globalPos = ui->assetsContentView->mapToGlobal(pos);
        contextMenu->ShowPopup(globalPos);

        contextMenu->adjustSize();
        contextMenu->resize(contextMenu->minimumSize());
    }

    void AssetsView::ReimportAssets(Array<AssetDatabaseEntry*> assetsToReimport)
    {
        if (assetsToReimport.GetSize() == 0)
            return;

        auto projectDir = ProjectSettings::Get().GetEditorProjectDirectory();
        auto parentDir = (projectDir / assetsToReimport[0]->GetVirtualPath()).GetParentPath();

        Array<IO::Path> assetPaths{};
        for (auto asset : assetsToReimport)
        {
            auto path = projectDir / asset->GetVirtualPath();
            if (!path.Exists())
                continue;

            auto srcAssetPath = path.ReplaceExtension(asset->extension);
            if (!srcAssetPath.Exists())
                continue;

            assetPaths.Add(srcAssetPath);
        }

        if (assetImporterWindow != nullptr)
        {
            delete assetImporterWindow;
            assetImporterWindow = nullptr;
        }

        assetImporterWindow = new AssetImporterWindow(this);
        assetImporterWindow->setWindowFlag(::Qt::Tool, true);
        assetImporterWindow->SetImportOnlyMode(true);
        assetImporterWindow->SetAssets(assetPaths);
        assetImporterWindow->show();

        assetImporterWindow->SetAssetsDirectory(parentDir);
        assetImporterWindow->UpdateDetailsView();

        assetImporterWindow->adjustSize();
        assetImporterWindow->resize(assetImporterWindow->minimumSize());
        assetImporterWindow->setFocus();
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

        auto newFolderPath = gameAssetsDir / currentDirectory->GetVirtualPath() / "New Folder";

        while (newFolderPath.Exists())
        {
            newFolderPath = gameAssetsDir / currentDirectory->GetVirtualPath() / (newFolderPath.GetFilename().GetString() + " (1)");
        }

        IO::Path::CreateDirectories(newFolderPath);
    }

    void AssetsView::OnContextMenuRenamePressed()
    {
        const auto& projectSettings = ProjectSettings::Get();
        auto gameAssetsDir = projectSettings.GetEditorProjectDirectory();
        auto selection = ui->assetsContentView->selectionModel()->selectedIndexes();

        if (selection.size() != 1 || !gameAssetsDir.Exists())
        {
            return;
        }

        const QModelIndex& item = selection.at(0);
        itemDelegate->SetRenameItemIndex(item);

        UpdateContentView();
    }

    void AssetsView::OnContextMenuDuplicatePressed()
    {
        const auto& projectSettings = ProjectSettings::Get();
        auto gameAssetsDir = projectSettings.GetEditorProjectDirectory();
        auto selection = ui->assetsContentView->selectionModel()->selectedIndexes();

        if (selection.size() == 0 || !gameAssetsDir.Exists())
        {
            return;
        }

        for (const QModelIndex& item : selection)
        {
            AssetDatabaseEntry* entry = (AssetDatabaseEntry*)item.internalPointer();
            if (entry == nullptr || (entry->category != AssetDatabaseEntry::Category::GameAssets &&
                entry->category != AssetDatabaseEntry::Category::GameShaders))
                continue;
            auto path = gameAssetsDir / entry->GetVirtualPath();
            if (path.Exists())
            {
                if (path.GetExtension() == ".casset")
                {
                    Asset tempAsset{ "Temp" };
                    SerializedObject so{ Asset::Type(), &tempAsset };
                    so.Deserialize(path);
                    auto srcAssetPath = path.ReplaceExtension("." + tempAsset.GetAssetExtension());
                    if (srcAssetPath.Exists())
                    {
                        auto newSrcAssetPath = srcAssetPath.GetParentPath() /
                                (srcAssetPath.GetFilename().RemoveExtension().GetString() + " Copy" + srcAssetPath.GetExtension().GetString());
                        IO::Path::Copy(srcAssetPath, newSrcAssetPath);
                    }

                    auto targetFilename = path.GetFilename().RemoveExtension().GetString() + " Copy" + path.GetExtension().GetString();
                    auto targetPath = path.GetParentPath() / targetFilename;
                    IO::Path::Copy(path, targetPath);
                }
                else
                {
                    auto targetFilename = path.GetFilename().RemoveExtension().GetString() + " Copy" + path.GetExtension().GetString();
                    auto targetPath = path.GetParentPath() / targetFilename;
                    IO::Path::Copy(path, targetPath);
                }
            }
        }
    }

    void AssetsView::OnContextMenuMoveToPressed()
    {
        const auto& projectSettings = ProjectSettings::Get();
        auto gameAssetsDir = projectSettings.GetEditorProjectDirectory();
        auto selection = ui->assetsContentView->selectionModel()->selectedIndexes();

        if (selection.size() == 0 || !gameAssetsDir.Exists())
        {
            return;
        }

        auto firstItem = (AssetDatabaseEntry*)selection.at(0).internalPointer();

        auto dir = (gameAssetsDir / firstItem->GetVirtualPath()).GetParentPath().GetString();
        QString dirStr = QString(dir.GetCString());
        auto moveDirectoryStr = QFileDialog::getExistingDirectory(this, "Select a folder for new project", dirStr);

        if (moveDirectoryStr.isEmpty())
            return;

        IO::Path moveDirectory = IO::Path(moveDirectoryStr.toStdString());

        for (const QModelIndex& item : selection)
        {
            AssetDatabaseEntry* entry = (AssetDatabaseEntry*)item.internalPointer();
            if (entry == nullptr || (entry->category != AssetDatabaseEntry::Category::GameAssets &&
                                     entry->category != AssetDatabaseEntry::Category::GameShaders))
                continue;
            auto path = gameAssetsDir / entry->GetVirtualPath();
            if (!path.Exists())
                continue;

            if (path.GetExtension() == ".casset")
            {
                path.GetParentPath().IterateChildren([&](const IO::Path& p)
                {
                    if (path.GetFilename().RemoveExtension() == p.GetFilename().RemoveExtension())
                    {
                        IO::Path::Rename(p, moveDirectory / p.GetFilename());
                    }
                });
            }
            else
            {
                IO::Path::Rename(path, moveDirectory / path.GetFilename());
            }
        }
    }

    void AssetsView::OnContextMenuDeletePressed()
    {
        const auto& projectSettings = ProjectSettings::Get();
        auto gameAssetsDir = projectSettings.GetEditorProjectDirectory();
        auto selection = ui->assetsContentView->selectionModel()->selectedIndexes();
        
        if (selection.size() == 0 || !gameAssetsDir.Exists())
        {
            return;
        }
        
        for (const QModelIndex& item : selection)
        {
            AssetDatabaseEntry* entry = (AssetDatabaseEntry*)item.internalPointer();
            if (entry == nullptr || (entry->category != AssetDatabaseEntry::Category::GameAssets &&
                                     entry->category != AssetDatabaseEntry::Category::GameShaders))
                continue;
            auto path = gameAssetsDir / entry->GetVirtualPath();
            if (!path.Exists())
                continue;
            if (path.IsDirectory())
            {
                IO::Path::RemoveRecursively(path);
            }
            else
            {
                Array<IO::Path> pathsToRemove{};
                path.GetParentPath().IterateChildren([&](const IO::Path& p)
                {
                    if (path.GetFilename().RemoveExtension() == p.GetFilename().RemoveExtension() && !p.IsDirectory())
                    {
                        pathsToRemove.Add(p);
                    }
                });
                for (const auto& p : pathsToRemove)
                {
                    IO::Path::Remove(p);
                }
            }
        }
    }

    void AssetsView::OnAssetDatabaseUpdated()
    {
        QMetaObject::invokeMethod(this, [=]
        {
            auto selection = ui->folderTreeView->selectionModel()->selectedIndexes();
            
            if (selection.size() > 0)
            {
                emit folderModel->modelReset({});
                ui->folderTreeView->selectionModel()->select(selection.at(0), QItemSelectionModel::Select);
            }

            UpdateContentView();
        });
    }

    void AssetsView::OnNewSourceAssetAdded(IO::Path path)
    {
        return; // TODO: Do nothing for now
        if (path.Exists() && !path.ReplaceExtension(".casset").Exists())
        {
            if (assetImporterWindow != nullptr && assetImporterWindow->IsImportOnlyMode())
            {
                delete assetImporterWindow;
                assetImporterWindow = nullptr;
            }

            if (assetImporterWindow == nullptr)
            {
                assetImporterWindow = new AssetImporterWindow(this);
            }

            assetImporterWindow->SetAssetsDirectory(ProjectSettings::Get().GetEditorProjectDirectory() / "Game/Assets");
            assetImporterWindow->UpdateDetailsView();

            assetImporterWindow->show();
            assetImporterWindow->SetFiltersVisible(false);
            assetImporterWindow->ShowTreeView(false);
            assetImporterWindow->ShowProcessedFiles(false);
            assetImporterWindow->ShowUnprocessedFiles(true);
            assetImporterWindow->SetMenuBarVisible(false);
            assetImporterWindow->SetAutoClose(true);

            assetImporterWindow->setFocus();
        }
    }

    void AssetsView::OnFolderSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
    {
        UpdateContentView();
    }

    AssetsViewItemDelegate::AssetsViewItemDelegate(AssetsView* parent)
        : QItemDelegate(parent)
        , assetsView(parent)
    {

    }

    AssetsViewItemDelegate::~AssetsViewItemDelegate()
    {

    }

    QWidget* AssetsViewItemDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
    {
        if (index.isValid() && index.internalPointer() != nullptr)
        {
            auto itemView = new AssetViewItem(parent);
            auto entry = (AssetDatabaseEntry*)index.internalPointer();
            itemView->SetEntry(entry);
            itemView->SetRenameMode(index == renameIndex);
            QObject::connect(itemView, &AssetViewItem::OnNameInputEdited, [=](QString newName)
            {
                const auto& projectSettings = ProjectSettings::Get();
                auto gameAssetsDir = projectSettings.GetEditorProjectDirectory();
                String extension = "";
                String newNameStr = newName.toStdString();
                
                auto oldPath = gameAssetsDir / entry->GetVirtualPath();
                if (!oldPath.Exists())
                    return;
                if (!oldPath.IsDirectory())
                    extension = oldPath.GetExtension().GetString();

                assetsView->itemDelegate->renameIndex = {};

                auto newPath = oldPath.GetParentPath() / (String(newName.toStdString()) + extension);
                if (oldPath == newPath || newPath.Exists())
                    return;

                if (!oldPath.IsDirectory() && oldPath.GetExtension() == ".casset")
                {
                    oldPath.GetParentPath().IterateChildren([oldPath,&newNameStr](const IO::Path& p)
                    {
                        if (oldPath.GetFilename().RemoveExtension() == p.GetFilename().RemoveExtension() && p.GetExtension() != ".casset")
                        {
                            auto oldSourcePath = p;
                            auto newSourcePath = p.GetParentPath() / (newNameStr + p.GetFilename().GetExtension().GetString());
                            IO::Path::Rename(oldSourcePath, newSourcePath);
                            return;
                        }
                    });
                    IO::Path::Rename(oldPath, newPath);
                }
                else
                {
                    IO::Path::Rename(oldPath, newPath);
                }
            });
            return itemView;
        }
        return QItemDelegate::createEditor(parent, option, index);
    }

    void AssetsViewItemDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
    {
        auto editorWidget = qobject_cast<AssetViewItem*>(editor);
        if (editorWidget != nullptr && index.isValid() && index.internalPointer() != nullptr)
        {
            auto entry = (AssetDatabaseEntry*)index.internalPointer();
            editorWidget->SetEntry(entry);
            editorWidget->SetRenameMode(index == renameIndex);
        }
    }

    void AssetsViewItemDelegate::OnNameInputEdited(QString newString)
    {
        
    }

}

