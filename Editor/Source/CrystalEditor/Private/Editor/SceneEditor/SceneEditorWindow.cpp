#include "SceneEditorWindow.h"
#include "ui_SceneEditorWindow.h"

#include "ViewportView/ViewportView.h"
#include "SceneOutlinerView/SceneOutlinerView.h"

#include <QFileDialog>

namespace CE::Editor
{

    SceneEditorWindow::SceneEditorWindow(QWidget* parent)
        : EditorWindowBase(parent)
        , ui(new Ui::SceneEditorWindow)
    {
        ui->setupUi(this);
        scenePath = "";

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

        // Scene Outliner connections
        connect(sceneOutlinerView, &SceneOutlinerView::CreateEmptyGameObject, this, &SceneEditorWindow::CreateEmptyGameObject);
        
        // TODO: Test code
        //editorScene->AddGameObject(new GameObject("GO 1"));
        //auto go2 = new GameObject("GO 2");
        //editorScene->AddGameObject(go2);
        //go2->AddChild(new GameObject("Child 1"));
        //go2->AddChild(new GameObject("Child 2"));
        
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
        scenePath = "";
        
        sceneOutlinerView->GetModel()->OnSceneOpened(editorScene);
    }

    void SceneEditorWindow::CreateEmptyGameObject()
    {
        editorScene->AddGameObject(new GameObject("Empty"));

        sceneOutlinerView->Update();
    }

    void SceneEditorWindow::on_actionNewScene_triggered()
    {
        OpenEmptyScene();
    }


    void SceneEditorWindow::on_actionSaveScene_triggered()
    {
        if (scenePath.IsEmpty())
        {
            on_actionSaveSceneAs_triggered();
            return;
        }

        IO::Path savePath = ProjectManager::Get().GetProjectBaseDirectory() / scenePath;

        if (!savePath.Exists())
        {
            on_actionSaveSceneAs_triggered();
            return;
        }

        SerializedObject sceneSO{ editorScene };
        sceneSO.Serialize(savePath);
    }


    void SceneEditorWindow::on_actionSaveSceneAs_triggered()
    {
        String savePath = (ProjectManager::Get().GetProjectBaseDirectory() / scenePath).GetString();
        QString savePathQString = savePath.GetCString();

        QFileDialog* saveFileDialog = new QFileDialog(this);
        QString saveFileFinalPath = saveFileDialog->getSaveFileName(this, "Save Scene As...", savePathQString, "*.cscene");

        if (!saveFileFinalPath.isEmpty())
        {
            if (!saveFileFinalPath.endsWith(".cscene"))
                saveFileFinalPath += ".cscene";
            
            SerializedObject so{ editorScene };
            so.Serialize(IO::Path(saveFileFinalPath.toStdString()));

            sceneOutlinerView->Update();

            CE_LOG(Info, All, "Saving scene at path: {}", saveFileFinalPath);
        }
    }

    void SceneEditorWindow::on_actionOpenScene_triggered()
    {
        String openPath = (ProjectManager::Get().GetProjectBaseDirectory() / scenePath).GetString();
        QString openPathQString = openPath.GetCString();

        QFileDialog* openFileDialog = new QFileDialog(this);
        QString openFileFinalPath = openFileDialog->getOpenFileName(this, "Open Scene", openPathQString, "*.cscene");

        if (!openFileFinalPath.isEmpty())
        {
            if (!openFileFinalPath.endsWith(".cscene"))
                return;

            OpenEmptyScene();

            SerializedObject so{ editorScene };
            so.Deserialize(IO::Path(openFileFinalPath.toStdString()));

            sceneOutlinerView->Update();

            CE_LOG(Info, All, "Opening scene at path: {}", openFileFinalPath);
        }
    }


}
