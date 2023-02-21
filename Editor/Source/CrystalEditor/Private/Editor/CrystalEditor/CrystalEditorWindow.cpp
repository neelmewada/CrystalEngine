#include "CrystalEditorWindow.h"
#include "ui_CrystalEditorWindow.h"

#include "Editor/SceneEditor/SceneEditorWindow.h"

#include <QLabel>

namespace CE::Editor
{
    CrystalEditorWindow::CrystalEditorWindow(QWidget* parent) :
        QMainWindow(parent),
        ui(new Ui::CrystalEditorWindow)
    {
        ui->setupUi(this);

        setWindowTitle("Crystal Editor");

        setMinimumWidth(1280);
        setMinimumHeight(720);

        using namespace ads;

        CDockManager::setConfigFlag(CDockManager::DockAreaHasUndockButton, false);
        CDockManager::setConfigFlag(CDockManager::DockAreaHasTabsMenuButton, false);
        CDockManager::setConfigFlag(CDockManager::DockAreaHasCloseButton, false);

        mainDockManager = new ads::CDockManager(this);

        SceneEditorWindow* sceneEditor = new SceneEditorWindow();

        ads::CDockWidget* dockWidget = new ads::CDockWidget(sceneEditor->windowTitle());
        dockWidget->setWidget(sceneEditor);

        dockWidgets.Add(dockWidget);
        
        mainDockManager->addDockWidget(ads::TopDockWidgetArea, dockWidget);
    }

    CrystalEditorWindow::~CrystalEditorWindow()
    {
        for (auto dockWidget : dockWidgets)
        {
            mainDockManager->removeDockWidget(dockWidget);
            delete dockWidget;
        }

        delete mainDockManager;
        delete ui;
    }

    void CrystalEditorWindow::LoadProject(IO::Path projectPath)
    {
        
    }

}
