#include "AssetImporterWindow.h"
#include "ui_AssetImporterWindow.h"

#include "EditorCore.h"
#include "AssetProcessor.h"

#include <QFileDialog>
#include <QPainter>

namespace CE::Editor
{
    AssetImporterWindow::AssetImporterWindow(QWidget* parent) :
        QMainWindow(parent),
        ui(new Ui::AssetImporterWindow)
    {
        ui->setupUi(this);
        setWindowFlag(::Qt::Tool, true);

        setWindowTitle("Asset Importer");

        ui->splitter->setSizes({ 200, 180 });

        ui->incompatibleSelectionLabel->setVisible(false);

        ui->importButton->setVisible(false);
        ui->resetButton->setVisible(false);
    }

    AssetImporterWindow::~AssetImporterWindow()
    {
        for (auto asset : assetClassInstances)
        {
            delete asset;
        }
        assetClassInstances.Clear();
        //delete assetClassInstance;
        delete ui;
    }

    void AssetImporterWindow::SetImportOnlyMode(bool set)
    {
        importOnlyMode = set;

        ui->contentTreeView->clearSelection();
        selection.Clear();

        ui->contentTreeParent->setVisible(!importOnlyMode);
        ui->menubar->setVisible(!importOnlyMode);
        ui->incompatibleSelectionLabel->setVisible(false);
    }

    void AssetImporterWindow::SetAssetsDirectory(IO::Path directory)
    {
        this->currentDirectory = directory;

        bool firstTime = false;

        if (model == nullptr)
        {
            firstTime = true;
            model = new AssetImporterFileModel(currentDirectory, this);
        }

        if (filterModel == nullptr)
        {
            filterModel = new AssetImporterFileFilterModel(this);
        }

        filterModel->setSourceModel(model);
        ui->contentTreeView->setModel(filterModel);

        model->SetTreeView(ui->showTreeView->isChecked());
        model->SetDirectory(currentDirectory);
        model->UpdateDirectoryData();

        if (firstTime)
        {
            QObject::connect(ui->contentTreeView->selectionModel(), &QItemSelectionModel::selectionChanged,
                this, &AssetImporterWindow::OnFileSelectionChanged);
        }

        TryClosingIfNoUnprocessedFiles();
    }

    void AssetImporterWindow::SetAssets(CE::Array<IO::Path> assetPaths)
    {
        this->targetAssetPaths = assetPaths;
    }

    void AssetImporterWindow::UpdateSelection()
    {
        selection.Clear();

        auto list = ui->contentTreeView->selectionModel()->selectedIndexes();

        for (int i = 0; i < list.size(); i++)
        {
            auto index = list.at(i);
            if (filterModel != nullptr)
                index = filterModel->mapToSource(index);
            if (index.internalPointer() == nullptr)
                continue;
            
            AssetImporterFileModelEntry* entry = (AssetImporterFileModelEntry*)index.internalPointer();
            if (entry->IsTerminal())
            {
                selection.Add(entry);
            }
        }

        UpdateDetailsView();
    }

    void AssetImporterWindow::UpdateDetailsView()
    {
        ui->importButton->setVisible(false);
        ui->resetButton->setVisible(false);
        
        for (auto fieldDrawer : fieldDrawers)
        {
            fieldDrawer->OnDisable();
            delete fieldDrawer;
        }
        fieldDrawers.Clear();

        // Clear the component container
        auto layout = ui->detailsPanelContent->layout();
        QLayoutItem* item;
        while ((item = layout->takeAt(0)) != NULL)
        {
            delete item->widget();
            delete item;
        }

        ui->incompatibleSelectionLabel->setVisible(false);

        if (!importOnlyMode && selection.GetSize() == 0)
            return;

        assetClass = nullptr;

        if (!importOnlyMode)
        {
            for (auto entry : selection)
            {
                auto extension = entry->GetExtension();
                if (extension.IsEmpty())
                    return;
                extension = extension.GetSubstring(1);
                auto curAssetClass = Asset::GetAssetClassFor(extension);
                if (assetClass != nullptr && assetClass != curAssetClass)
                {
                    ui->incompatibleSelectionLabel->setVisible(true);
                    return;
                }

                assetClass = curAssetClass;
            }
        }
        else
        {
            for (auto assetPath : targetAssetPaths)
            {
                if (assetPath.IsDirectory() || !assetPath.Exists())
                    continue;

                auto curAssetClass = Asset::GetAssetClassFor(assetPath.GetExtension().GetString());
                if (assetClass != nullptr && curAssetClass != assetClass)
                    return;

                assetClass = curAssetClass;
            }
        }

        if (importOnlyMode && assetClassInstances.GetSize() > 0)
        {
            for (auto inst : assetClassInstances)
            {
                delete inst;
            }
            assetClassInstances.Clear();
        }

        if (assetClass == nullptr)
            return;

        /*if (assetClassInstance != nullptr && assetClassInstance->GetType()->GetTypeId() != assetClass->GetTypeId())
        {
            delete assetClassInstance;
            assetClassInstance = nullptr;
        }*/

        /*if (assetClassInstance == nullptr)
        {
            assetClassInstance = (Asset*)assetClass->CreateDefaultInstance();
        }*/

        if (importOnlyMode)
        {
            for (int i = 0; i < targetAssetPaths.GetSize(); i++)
            {
                while (i >= assetClassInstances.GetSize())
                    assetClassInstances.Add((Asset*)assetClass->CreateDefaultInstance());

                auto assetPath = targetAssetPaths[i];
                auto productAssetPath = assetPath.ReplaceExtension(".casset");
                if (assetPath.IsDirectory() || !assetPath.Exists() || !productAssetPath.Exists())
                    continue;

                SerializedObject so{ assetClass, assetClassInstances[i] };
                so.Deserialize(productAssetPath);
                break;
            }
        }
        else
        {
            for (auto entry : selection)
            {
                auto extension = entry->GetExtension();
                if (extension.IsEmpty())
                    return;
                extension = extension.GetSubstring(1);
                auto curAssetClass = Asset::GetAssetClassFor(extension);
                if (assetClass != nullptr && assetClass != curAssetClass)
                {
                    ui->incompatibleSelectionLabel->setVisible(true);
                    return;
                }

                auto inst = (Asset*)assetClass->CreateDefaultInstance();
                assetClassInstances.Add(inst);

                auto assetPath = entry->fullPath;
                auto productAssetPath = assetPath.ReplaceExtension(".casset");
                if (!productAssetPath.Exists())
                    continue;

                SerializedObject so{ assetClass, inst };
                so.Deserialize(productAssetPath);
            }
        }

        auto field = assetClass->GetFirstField();

        Array<void*> instanceArray{};
        for (auto inst : assetClassInstances)
        {
            instanceArray.Add(inst);
        }

        while (field != nullptr)
        {
            if (field->IsHidden())
            {
                field = field->GetNext();
                continue;
            }
            if (!field->HasLocalAttribute("ImportSetting"))
            {
                field = field->GetNext();
                continue;
            }

            auto fieldDrawerType = FieldDrawer::GetFieldDrawerClassFor(field->GetTypeId());

            if (fieldDrawerType == nullptr)
            {
                field = field->GetNext();
                continue;
            }

            if (!fieldDrawerType->CanBeInstantiated())
            {
                field = field->GetNext();
                continue;
            }

            auto fieldDrawer = (FieldDrawer*)fieldDrawerType->CreateDefaultInstance();

            if (fieldDrawer == nullptr)
            {
                field = field->GetNext();
                continue;
            }

            fieldDrawer->SetTargets(field, instanceArray);

            fieldDrawers.Add(fieldDrawer);

            fieldDrawer->OnEnable();

            fieldDrawer->CreateGUI(ui->detailsPanelContent->layout());

            field = field->GetNext();
        }
        
        ui->importButton->setVisible(true);
        ui->resetButton->setVisible(true);
    }

    void AssetImporterWindow::SetFiltersVisible(bool visible)
    {
        ui->showTreeView->setVisible(visible);
        ui->showProcessedFiles->setVisible(visible);
        ui->showUnprocessedFiles->setVisible(visible);
    }

    void AssetImporterWindow::SetAutoClose(bool autoClose)
    {
        this->autoClose = autoClose;
    }

    void AssetImporterWindow::ShowTreeView(bool treeView)
    {
        if (model == nullptr)
            return;

        model->SetTreeView(treeView);
        model->modelReset({});
    }

    void AssetImporterWindow::ShowUnprocessedFiles(bool show)
    {
        if (filterModel == nullptr)
            return;

        filterModel->showUnprocessedFiles = show;
        emit model->modelReset({});
        emit filterModel->modelReset({});
    }

    void AssetImporterWindow::ShowProcessedFiles(bool show)
    {
        if (filterModel == nullptr)
            return;

        filterModel->showProcessedFiles = show;
        emit model->modelReset({});
        emit filterModel->modelReset({});
    }

    void AssetImporterWindow::SetMenuBarVisible(bool visible)
    {
        ui->menubar->setVisible(visible);
    }

    void AssetImporterWindow::paintEvent(QPaintEvent* event)
    {
        QPainter p(this);
        p.setPen(::Qt::NoPen);
        p.setBrush(QBrush(qApp->palette().color(QPalette::AlternateBase)));
        p.drawRect(this->rect());

        QMainWindow::paintEvent(event);
    }

    void AssetImporterWindow::TryClosingIfNoUnprocessedFiles()
    {
        if (importOnlyMode || model == nullptr || !autoClose)
            return;

        if (model->allFileEntries.GetSize() == 0)
        {
            close();
            return;
        }

        int numUnprocessedFiles = 0;

        for (auto entry : model->allFileEntries)
        {
            if (!entry->IsProcessed())
            {
                numUnprocessedFiles++;
            }
        }

        if (numUnprocessedFiles == 0)
        {
            close();
        }
    }

    void AssetImporterWindow::OnFileSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
    {
        UpdateSelection();
    }

    void AssetImporterWindow::on_actionOpenFolder_triggered()
    {
        auto launchDir = PlatformDirectories::GetLaunchDir().GetString();
        QString folderPath = QFileDialog::getExistingDirectory(this, "Select an assets folder", launchDir.GetCString());

        if (folderPath.isEmpty())
        {
            return;
        }

        SetAssetsDirectory(folderPath.toStdString());
    }


    void AssetImporterWindow::on_showTreeView_stateChanged(int checked)
    {
        ShowTreeView(checked);
    }


    void AssetImporterWindow::on_showUnprocessedFiles_stateChanged(int checked)
    {
        ShowUnprocessedFiles(checked);
    }


    void AssetImporterWindow::on_showProcessedFiles_stateChanged(int checked)
    {
        ShowProcessedFiles(checked);
    }


    void AssetImporterWindow::on_importButton_clicked()
    {
        if (importOnlyMode && targetAssetPaths.GetSize() > 0 && assetClassInstances.GetSize() > 0)
        {
            for (int i = 0; i < targetAssetPaths.GetSize(); i++)
            {
                auto entry = targetAssetPaths[i];
                if (!entry.Exists() || entry.IsDirectory() || i >= assetClassInstances.GetSize())
                    continue;

                AssetProcessor::Get().ProcessAsset(entry, assetClassInstances[i]);
            }

            targetAssetPaths.Clear();
            close();
            return;
        }

        if (selection.GetSize() == 0 || assetClass == nullptr || assetClassInstances.GetSize() == 0)
            return;

        for (int i = 0; i < selection.GetSize(); i++)
        {
            auto entry = selection[i];
            if (entry == nullptr)
                continue;

            AssetProcessor::Get().ProcessAsset(entry->fullPath, assetClassInstances[i]);
        }

        ui->contentTreeView->clearSelection();
        selection.Clear();
        
        emit model->modelReset({});
        emit filterModel->modelReset({});
        
        UpdateDetailsView();

        if (CanAutoClose())
        {
            close();
        }
    }

    void AssetImporterWindow::on_resetButton_clicked()
    {
        if (selection.GetSize() == 0 || assetClass == nullptr || assetClassInstances.GetSize() == 0)
            return;

        for (auto inst : assetClassInstances)
        {
            assetClass->InitializeDefaults(inst);
        }
        
        for (auto fieldDrawer : fieldDrawers)
        {
            fieldDrawer->OnValuesUpdated();
        }

        if (importOnlyMode && targetAssetPaths.GetSize() > 0)
        {
            for (auto path : targetAssetPaths)
            {
                if (path.ReplaceExtension(".casset").Exists())
                {
                    IO::Path::Remove(path.ReplaceExtension(".casset"));
                }
            }
            targetAssetPaths.Clear();
        }
        else if (selection.GetSize() > 0)
        {
            for (auto entry : selection)
            {
                if (entry->fullPath.ReplaceExtension(".casset").Exists())
                {
                    IO::Path::Remove(entry->fullPath.ReplaceExtension(".casset"));
                }
            }
        }

        ui->contentTreeView->clearSelection();
        selection.Clear();

        UpdateDetailsView();
    }

}
