#include "AssetImporterWindow.h"
#include "ui_AssetImporterWindow.h"

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
    }

    AssetImporterWindow::~AssetImporterWindow()
    {
        delete ui;
    }

    void AssetImporterWindow::on_actionOpenFolder_triggered()
    {
        auto launchDir = PlatformDirectories::GetLaunchDir().GetString();
        QString folderPath = QFileDialog::getExistingDirectory(this, "Select a folder for new project", launchDir.GetCString());

        if (folderPath.isEmpty())
        {
            return;
        }

        currentDirectory = folderPath.toStdString();

        if (model == nullptr)
        {
            model = new AssetImporterFileModel(currentDirectory, this);
        }

        model->SetDirectory(currentDirectory);
        model->UpdateDirectoryData();

        ui->contentTreeView->setModel(model);
    }


    void AssetImporterWindow::on_showTreeView_stateChanged(int checked)
    {
        model->SetTreeView(checked);
    }

}
