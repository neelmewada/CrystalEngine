#include "SceneEditorWindow.h"
#include "ui_SceneEditorWindow.h"

#include "ViewportView/ViewportView.h"
#include "SceneOutlinerView/SceneOutlinerView.h"

namespace CE::Editor
{

    SceneEditorWindow::SceneEditorWindow(QWidget* parent)
        : EditorWindowBase(parent)
        , ui(new Ui::SceneEditorWindow)
    {
        ui->setupUi(this);

        setWindowTitle("Scene Editor");
        
        using namespace ads;
        
        CDockManager::setConfigFlag(CDockManager::DockAreaHasUndockButton, false);
        CDockManager::setConfigFlag(CDockManager::DockAreaHasTabsMenuButton, false);
        CDockManager::setConfigFlag(CDockManager::DockAreaHasCloseButton, false);

        dockManager = new CDockManager(this);

        // Viewport
        viewportView = new ViewportView();
        ads::CDockWidget* viewportViewDockWidget = new ads::CDockWidget(viewportView->windowTitle());
        viewportViewDockWidget->setWidget(viewportView);
        
        // Scene Outliner
        sceneOutlinerView = new SceneOutlinerView();
        ads::CDockWidget* sceneOutlinerDockWidget = new ads::CDockWidget(sceneOutlinerView->windowTitle());
        sceneOutlinerDockWidget->setWidget(sceneOutlinerView);
        
        // TODO: Test code
//        editorScene->AddGameObject(new GameObject("GO 1"));
//        auto go2 = new GameObject("GO 2");
//        editorScene->AddGameObject(go2);
//        go2->AddChild(new GameObject("Child 1"));
//        go2->AddChild(new GameObject("Child 2"));
        
        // Set models
        sceneOutlinerView->SetModel(new SceneOutlinerModel(this));
        sceneOutlinerView->GetModel()->OnSceneOpened(editorScene);

        // Add the dock widget to the top dock widget area
        dockManager->addDockWidget(ads::CenterDockWidgetArea, viewportViewDockWidget);
        dockManager->addDockWidget(ads::LeftDockWidgetArea, sceneOutlinerDockWidget);
    }

    SceneEditorWindow::~SceneEditorWindow()
    {
        delete ui;
    }

    void SceneEditorWindow::OpenEmptyScene()
    {
        delete editorScene;
        editorScene = new Scene();
        
        sceneOutlinerView->GetModel()->OnSceneOpened(editorScene);
    }

}
