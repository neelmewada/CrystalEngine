#ifndef PROJECTBROWSER_H
#define PROJECTBROWSER_H

#include <QDialog>

namespace Ui {
class ProjectBrowser;
}

namespace CE::Editor
{

    class ProjectBrowser : public QDialog
    {
        Q_OBJECT

    public:
        explicit ProjectBrowser(QWidget* parent = nullptr);
        ~ProjectBrowser();

    private slots:

        void on_openSelectFolderButton_clicked();

        void on_createProjectButtonBox_accepted();

        void on_createProjectButtonBox_rejected();

        void on_openProjectButtonBox_accepted();

        void on_openProjectButtonBox_rejected();

        void on_openSelectProjectButton_clicked();

    private:
        Ui::ProjectBrowser* ui;
    };

}

#endif // PROJECTBROWSER_H
