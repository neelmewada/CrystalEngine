#include "AssetImporterWindow.h"
#include "ui_AssetImporterWindow.h"

#include "EditorCore.h"
#include "AssetProcessor.h"

#include <QFileDialog>

namespace CE::Editor
{
    AssetImporterWindow::AssetImporterWindow(QWidget* parent) :
        QMainWindow(parent),
        ui(new Ui::AssetImporterWindow)
    {
        ui->setupUi(this);

        setWindowTitle("Asset Importer");

        ui->splitter->setSizes({ 200, 180 });

        ui->incompatibleSelectionLabel->setVisible(false);

        ui->importButton->setVisible(false);
        ui->resetButton->setVisible(false);
    }

    AssetImporterWindow::~AssetImporterWindow()
    {
        delete assetClassInstance;
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
    }

    void AssetImporterWindow::SetAssetsPath(CE::Array<IO::Path> assetPaths)
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

        if (selection.GetSize() == 0)
            return;

        assetClass = nullptr;

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

        if (assetClass == nullptr)
            return;

        if (assetClassInstance != nullptr && assetClassInstance->GetType()->GetTypeId() != assetClass->GetTypeId())
        {
            delete assetClassInstance;
            assetClassInstance = nullptr;
        }

        if (assetClassInstance == nullptr)
        {
            assetClassInstance = (Asset*)assetClass->CreateDefaultInstance();
        }

        auto field = assetClass->GetFirstField();

        while (field != nullptr)
        {
            if (field->IsHidden())
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

            FieldDrawer* fieldDrawer = (FieldDrawer*)fieldDrawerType->CreateDefaultInstance();

            if (fieldDrawer == nullptr)
            {
                field = field->GetNext();
                continue;
            }

            fieldDrawer->SetTarget(field, assetClassInstance);

            fieldDrawers.Add(fieldDrawer);

            fieldDrawer->OnEnable();

            fieldDrawer->CreateGUI(ui->detailsPanelContent->layout());

            field = field->GetNext();
        }
        
        ui->importButton->setVisible(true);
        ui->resetButton->setVisible(true);
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
        if (model == nullptr)
            return;

        model->SetTreeView(checked);
        model->modelReset({});
    }


    void AssetImporterWindow::on_showUnprocessedFiles_stateChanged(int checked)
    {
        if (filterModel == nullptr)
            return;

        filterModel->showUnprocessedFiles = checked;
        emit model->modelReset({});
        emit filterModel->modelReset({});
    }


    void AssetImporterWindow::on_showProcessedFiles_stateChanged(int checked)
    {
        if (filterModel == nullptr)
            return;

        filterModel->showProcessedFiles = checked;
        emit model->modelReset({});
        emit filterModel->modelReset({});
    }


    void AssetImporterWindow::on_importButton_clicked()
    {
        if (selection.GetSize() == 0 || assetClass == nullptr || assetClassInstance == nullptr)
            return;
        
        for (auto entry : selection)
        {
            if (entry == nullptr)
                continue;
            
            AssetProcessor::Get().ProcessAsset(entry->fullPath, assetClassInstance);
        }
        
        ui->contentTreeView->clearSelection();
        selection.Clear();
        
        emit model->modelReset({});
        emit filterModel->modelReset({});
        
        UpdateDetailsView();
    }

    void AssetImporterWindow::on_resetButton_clicked()
    {
        if (selection.GetSize() == 0 || assetClass == nullptr || assetClassInstance == nullptr)
            return;
        
        assetClass->InitializeDefaults(assetClassInstance);
        
        for (auto fieldDrawer : fieldDrawers)
        {
            fieldDrawer->OnValuesUpdated();
        }
    }

}