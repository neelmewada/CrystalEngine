#include "SceneEditorWindow.h"
#include "ui_SceneEditorWindow.h"

#include "ViewportView/ViewportView.h"
#include "SceneOutlinerView/SceneOutlinerView.h"
#include "DetailsView/DetailsView.h"
#include "ConsoleView/ConsoleView.h"

#include "Editor/CrystalEditor/AssetsView/AssetsView.h"

#include <QFileDialog>

namespace CE::Editor
{

    SceneEditorWindow::SceneEditorWindow(QWidget* parent)
        : EditorWindowBase(parent)
        , ui(new Ui::SceneEditorWindow)
    {
        CE_CONNECT(SceneEditorBus, this);

        ui->setupUi(this);
        scenePath = "";

        setWindowTitle("Scene Editor");
        
        using namespace ads;
        
        CDockManager::setConfigFlag(CDockManager::DockAreaHasUndockButton, false);
        CDockManager::setConfigFlag(CDockManager::DockAreaHasTabsMenuButton, false);
        CDockManager::setConfigFlag(CDockManager::DockAreaHasCloseButton, false);

        dockManager = new CDockManager(this);

        // **********************************
        // Viewport View
        viewportView = new ViewportView(this);
        viewportViewDockWidget = new ads::CDockWidget(viewportView->windowTitle());
        viewportViewDockWidget->setWidget(viewportView);
        
        // **********************************
        // Scene Outliner View
        sceneOutlinerView = new SceneOutlinerView(this);
        sceneOutlinerViewDockWidget = new ads::CDockWidget(sceneOutlinerView->windowTitle());
        sceneOutlinerViewDockWidget->setWidget(sceneOutlinerView);

        // Scene Outliner connections
        connect(sceneOutlinerView, &SceneOutlinerView::CreateEmptyGameObject, this, &SceneEditorWindow::on_createEmptyGameObject_triggered);
        
        // Set models
        sceneOutlinerView->SetModel(new SceneOutlinerModel(this));
        sceneOutlinerView->GetModel()->OnSceneOpened(editorScene);

        // **********************************
        // Details View
        detailsView = new DetailsView(this);
        detailsViewDockWidget = new ads::CDockWidget(detailsView->windowTitle());
        detailsViewDockWidget->setWidget(detailsView);

        // Details View connections
        connect(sceneOutlinerView, &SceneOutlinerView::OnSceneSelectionChanged, detailsView, &DetailsView::OnSceneSelectionChanged);
        connect(detailsView, &DetailsView::GameObjectEntriesNeedRefresh, sceneOutlinerView, &SceneOutlinerView::UpdateSelectedGameObjectEntries);

        // **********************************
        // Console View
        consoleView = new ConsoleView(this);
        consoleViewDockWidget = new ads::CDockWidget(consoleView->windowTitle());
        consoleViewDockWidget->setWidget(consoleView);

        // Console View connections
        connect(consoleView, &ConsoleView::OnLogPushed, this, &SceneEditorWindow::OnLogPushedToConsole);

        // **********************************
        // Assets View
        assetsView = new AssetsView(this);
        assetsViewDockWidget = new ads::CDockWidget(assetsView->windowTitle());
        assetsViewDockWidget->setWidget(assetsView);

        // **********************************
        // Add the dock widget to the top dock widget area
        dockManager->addDockWidget(ads::CenterDockWidgetArea, viewportViewDockWidget);
        dockManager->addDockWidget(ads::LeftDockWidgetArea, sceneOutlinerViewDockWidget);
        dockManager->addDockWidget(ads::RightDockWidgetArea, detailsViewDockWidget);
        auto consoleDockArea = dockManager->addDockWidget(ads::BottomDockWidgetArea, consoleViewDockWidget);
        dockManager->addDockWidgetTab(ads::BottomDockWidgetArea, assetsViewDockWidget);
    }

    SceneEditorWindow::~SceneEditorWindow()
    {
        CE_DISCONNECT(SceneEditorBus, this);

        delete ui;
    }

    void SceneEditorWindow::OpenEmptyScene()
    {
        sceneOutlinerView->ClearSelection();

        delete editorScene;
        editorScene = new Scene();
        scenePath = "";
        
        sceneOutlinerView->GetModel()->OnSceneOpened(editorScene);

        detailsView->selection.Clear();
        
        sceneOutlinerView->Refresh();
        detailsView->Refresh();
    }

    void SceneEditorWindow::CreateEmptyGameObject()
    {
        auto go = new GameObject("Empty");
        auto selection = sceneOutlinerView->GetTreeView()->selectionModel()->selectedIndexes();

        GameObject* parent = nullptr;
        int parentSiblingIdx = 0;
        int goAddIndex = 0;

        if (selection.size() == 0)
        {
            goAddIndex = editorScene->GetRootGameObjectCount();
            editorScene->AddGameObject(go);
        }
        else
        {
            auto idx = selection.at(0);
            if (idx.isValid() && idx.internalPointer() != nullptr)
            {
                GameObject* selected = (GameObject*)idx.internalPointer();
                goAddIndex = selected->GetChildrenCount();
                selected->AddChild(go);
                parent = selected;

                if (parent->GetParent() != nullptr)
                {
                    parentSiblingIdx = parent->GetParent()->GetChildIndex(parent);
                }
                else
                {
                    parentSiblingIdx = editorScene->GetRootGameObjectIndex(parent);
                }
            }
            else
            {
                goAddIndex = editorScene->GetRootGameObjectCount();
                editorScene->AddGameObject(go);
            }
        }

        auto model = sceneOutlinerView->GetModel();
        emit model->rowsInserted(parent != nullptr ? model->CreateIndex(parentSiblingIdx, 0, parent) : QModelIndex(), goAddIndex, 0, {});
    }

    void SceneEditorWindow::OpenScene(String sceneAssetPath)
    {
        String openPath = (ProjectManager::Get().GetProjectBaseDirectory() / sceneAssetPath).GetString();
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

            sceneOutlinerView->Refresh();
            detailsView->Refresh();

            CE_LOG(Info, All, "Opening scene at path: {}", openFileFinalPath);
        }
    }

    // SLOTS

    void SceneEditorWindow::OnLogPushedToConsole(const String& string)
    {
        QString message = QString(string.GetCString());
        ui->statusBar->showMessage(message);
    }

    void SceneEditorWindow::on_createEmptyGameObject_triggered()
    {
        CreateEmptyGameObject();
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

            CE_LOG(Info, All, "Saving scene at path: {}", saveFileFinalPath);
        }
    }

    void SceneEditorWindow::on_actionOpenScene_triggered()
    {
        OpenScene(scenePath);
    }

    void SceneEditorWindow::on_actionExit_triggered()
    {
        qApp->quit();
    }

    void SceneEditorWindow::on_actionConsole_triggered()
    {
        if (!consoleViewDockWidget->isVisible())
        {
            consoleViewDockWidget->toggleView(true);
        }
    }

    void SceneEditorWindow::on_actionSceneOutliner_triggered()
    {
        if (!sceneOutlinerViewDockWidget->isVisible())
        {
            sceneOutlinerViewDockWidget->toggleView(true);
        }
    }

    void SceneEditorWindow::on_actionDetails_triggered()
    {
        if (!detailsViewDockWidget->isVisible())
        {
            detailsViewDockWidget->toggleView(true);
        }
    }

    void SceneEditorWindow::on_actionViewport_triggered()
    {
        if (!viewportViewDockWidget->isVisible())
        {
            viewportViewDockWidget->toggleView(true);
        }
    }

}

CE_RTTI_CLASS_IMPL(CRYSTALEDITOR_API, CE::Editor, SceneEditorWindow)
