#include "ProjectBrowser.h"
#include "ui_ProjectBrowser.h"

#include <QFileDialog>

#include "Events/CrystalEditorEventBus.h"

namespace CE::Editor
{
    ProjectBrowser::ProjectBrowser(QWidget* parent) :
        QDialog(parent),
        ui(new Ui::ProjectBrowser)
    {
        ui->setupUi(this);
    }

    ProjectBrowser::~ProjectBrowser()
    {
        delete ui;
    }

    void ProjectBrowser::on_openSelectFolderButton_clicked()
    {
        QString folderPath = QFileDialog::getExistingDirectory(this, "Select a folder for new project");
        
        if (!folderPath.isEmpty())
        {
            ui->projectFolderTextBox->setText(folderPath);
        }
    }


    void ProjectBrowser::on_createProjectButtonBox_accepted()
    {
        MBUS_EVENT(CrystalEditorEventBus, OnCreateProject, IO::Path(ui->projectFolderTextBox->text().toStdString()), String(ui->projectNameTextBox->text().toStdString())); // TODO: Placeholder empty project path
    }


    void ProjectBrowser::on_createProjectButtonBox_rejected()
    {
        close();
        qApp->quit();
    }


    void ProjectBrowser::on_openProjectButtonBox_accepted()
    {
        MBUS_EVENT(CrystalEditorEventBus, OnOpenProject, IO::Path(ui->openProjectPathTextBox->text().toStdString()));
    }


    void ProjectBrowser::on_openProjectButtonBox_rejected()
    {
        close();
        qApp->quit();
    }


    void ProjectBrowser::on_openSelectProjectButton_clicked()
    {
        QString projectFilePath = QFileDialog::getOpenFileName(this, "Select a Crystal Engine project", QString(), "*.cproject");

        if (!projectFilePath.isEmpty())
        {
            ui->openProjectPathTextBox->setText(projectFilePath);
        }
    }

}
