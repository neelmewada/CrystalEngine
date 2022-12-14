#include "SceneEditorWindow.h"
#include "ui_SceneEditorWindow.h"

#include "SceneView/SceneView.h"

namespace CE::Editor
{

    SceneEditorWindow::SceneEditorWindow(QWidget* parent) :
        QMainWindow(parent),
        ui(new Ui::SceneEditorWindow)
    {
        ui->setupUi(this);

        setWindowTitle("Scene Editor");

        using namespace ads;

        CDockManager::setConfigFlag(CDockManager::DockAreaHasUndockButton, false);
        CDockManager::setConfigFlag(CDockManager::DockAreaHasTabsMenuButton, false);
        CDockManager::setConfigFlag(CDockManager::DockAreaHasCloseButton, false);

        dockManager = new CDockManager(this);

        SceneView* sceneView = new SceneView();

        // Create a dock widget with the title Label 1 and set the created label
        // as the dock widget content
        ads::CDockWidget* dockWidget = new ads::CDockWidget(sceneView->windowTitle());
        dockWidget->setWidget(sceneView);

        // Add the dock widget to the top dock widget area
        dockManager->addDockWidget(ads::TopDockWidgetArea, dockWidget);
    }

    SceneEditorWindow::~SceneEditorWindow()
    {
        delete ui;
    }

}
