#ifndef ASSETPROCESSORWINDOW_H
#define ASSETPROCESSORWINDOW_H

#include <QMainWindow>

#include "CoreMinimal.h"
#include "System.h"

#include "AssetImporterFileModel.h"
#include "AssetImporterFileFilterModel.h"

namespace Ui {
class AssetImporterWindow;
}

namespace CE::Editor
{
    class FieldDrawer;

    class ASSETPROCESSOR_API AssetImporterWindow : public QMainWindow
    {
        Q_OBJECT
    public:
        explicit AssetImporterWindow(QWidget* parent = nullptr);
        ~AssetImporterWindow();

    public:

        void SetImportOnlyMode(bool set);
        bool IsImportOnlyMode() const { return importOnlyMode; }
        bool CanAutoClose() const { return autoClose || importOnlyMode; }

        void SetAssetsDirectory(IO::Path directory);
        void SetAssets(CE::Array<IO::Path> assetPaths);

        void UpdateSelection();

        void UpdateDetailsView();

        void SetFiltersVisible(bool visible);

        void SetAutoClose(bool autoClose);
        void ShowTreeView(bool treeView);
        void ShowUnprocessedFiles(bool show);
        void ShowProcessedFiles(bool show);

        void SetMenuBarVisible(bool visible);

    private:

        void TryClosingIfNoUnprocessedFiles();

    private slots:
        void OnFileSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);

        void on_actionOpenFolder_triggered();

        void on_showTreeView_stateChanged(int checked);

        void on_showUnprocessedFiles_stateChanged(int checked);

        void on_showProcessedFiles_stateChanged(int checked);

        void on_importButton_clicked();

        void on_resetButton_clicked();

    private:
        bool importOnlyMode = false;
        bool autoClose = false;

        IO::Path currentDirectory{};

        CE::Array<IO::Path> targetAssetPaths{};

        AssetImporterFileModel* model = nullptr;
        AssetImporterFileFilterModel* filterModel = nullptr;

        Array<AssetImporterFileModelEntry*> selection{};

        Array<FieldDrawer*> fieldDrawers{};

        // Import Settings
        ClassType* assetClass = nullptr;
        //Asset* assetClassInstance = nullptr;
        Array<Asset*> assetClassInstances{};

        Ui::AssetImporterWindow* ui;
    };

}

#endif // ASSETPROCESSORWINDOW_H
