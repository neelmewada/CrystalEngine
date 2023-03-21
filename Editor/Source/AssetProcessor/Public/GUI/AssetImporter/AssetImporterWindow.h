#ifndef ASSETPROCESSORWINDOW_H
#define ASSETPROCESSORWINDOW_H

#include <QMainWindow>

#include "CoreMinimal.h"

#include "AssetImporterFileModel.h"

namespace Ui {
class AssetImporterWindow;
}

namespace CE::Editor
{

    class ASSETPROCESSOR_API AssetImporterWindow : public QMainWindow
    {
        Q_OBJECT
    public:
        explicit AssetImporterWindow(QWidget* parent = nullptr);
        ~AssetImporterWindow();

    private slots:
        void on_actionOpenFolder_triggered();

        void on_showTreeView_stateChanged(int checked);

    private:
        IO::Path currentDirectory{};

        AssetImporterFileModel* model = nullptr;

        Ui::AssetImporterWindow* ui;
    };

}

#endif // ASSETPROCESSORWINDOW_H
