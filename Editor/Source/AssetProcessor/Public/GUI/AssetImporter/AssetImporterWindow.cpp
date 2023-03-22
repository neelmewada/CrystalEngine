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

        ui->splitter->setSizes({ 200, 150 });

        ui->incompatibleSelectionLabel->setVisible(false);
    }

    AssetImporterWindow::~AssetImporterWindow()
    {
        delete ui;
    }

    void AssetImporterWindow::UpdateSelection()
    {
        selection.Clear();

        auto list = ui->contentTreeView->selectionModel()->selectedIndexes();

        for (int i = 0; i < list.size(); i++)
        {
            const auto& index = list.at(i);
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

        ClassType* importSettingsClass = nullptr;

        for (auto entry : selection)
        {
            auto curImportSettingsClass = AssetImportSettings::GetImportSettingsClassFor(entry->GetExtension());
            if (importSettingsClass != nullptr && importSettingsClass != curImportSettingsClass)
            {
                ui->incompatibleSelectionLabel->setVisible(true);
                return;
            }

            importSettingsClass = curImportSettingsClass;
        }

        if (importSettingsClass == nullptr)
            return;

        auto field = importSettingsClass->GetFirstField();

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

            FieldDrawer* fieldDrawer = (FieldDrawer*)fieldDrawerType->CreateDefaultInstance();

            if (fieldDrawer == nullptr)
            {
                field = field->GetNext();
                continue;
            }

            fieldDrawer->SetTarget(field, nullptr);

            fieldDrawers.Add(fieldDrawer);

            fieldDrawer->OnEnable();

            fieldDrawer->CreateGUI(ui->detailsPanelContent->layout());

            field = field->GetNext();
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

        currentDirectory = folderPath.toStdString();
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

}
