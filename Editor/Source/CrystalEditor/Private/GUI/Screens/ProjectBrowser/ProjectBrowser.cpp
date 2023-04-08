#include "ProjectBrowser.h"
#include "ui_ProjectBrowser.h"

#include <QFileDialog>

#include "EditorSystem.h"

#include "Events/CrystalEditorEventBus.h"

namespace CE::Editor
{
    ProjectBrowser::ProjectBrowser(QWidget* parent) :
        QDialog(parent),
        ui(new Ui::ProjectBrowser)
    {
        ui->setupUi(this);

        auto path = EditorPrefs::Get().GetLatestProjectPath();

        if (!path.IsEmpty())
        {
            String pathStr = path.GetString();
            String parentPathStr = path.GetParentPath().GetString();
            ui->openProjectPathTextBox->setText(QString(pathStr.GetCString()));
            ui->projectFolderTextBox->setText(QString(parentPathStr.GetCString()));
        }
    }

    ProjectBrowser::~ProjectBrowser()
    {
        delete ui;
    }

    void ProjectBrowser::on_openSelectFolderButton_clicked()
    {
        QString folderPath = QFileDialog::getExistingDirectory(this, "Select a folder for new project", ui->projectFolderTextBox->text());
        
        if (!folderPath.isEmpty())
        {
            ui->projectFolderTextBox->setText(folderPath);
        }
    }

    void ProjectBrowser::on_createProjectButtonBox_accepted()
    {
        CE_PUBLISH(CrystalEditorEventBus, OnCreateProject, 
            IO::Path(ui->projectFolderTextBox->text().toStdString()), 
            String(ui->projectNameTextBox->text().toStdString()));
    }

    void ProjectBrowser::on_createProjectButtonBox_rejected()
    {
        close();
    }

    void ProjectBrowser::on_openProjectButtonBox_accepted()
    {
        CE_PUBLISH(CrystalEditorEventBus, OnOpenProject, IO::Path(ui->openProjectPathTextBox->text().toStdString()));
    }

    void ProjectBrowser::on_openProjectButtonBox_rejected()
    {
        close();
    }

    void ProjectBrowser::on_openSelectProjectButton_clicked()
    {
        QString projectFilePath = QFileDialog::getOpenFileName(this, "Select a Crystal Engine project", ui->openProjectPathTextBox->text(), "*.cproject");

        if (!projectFilePath.isEmpty())
        {
            ui->openProjectPathTextBox->setText(projectFilePath);
        }
    }

}
