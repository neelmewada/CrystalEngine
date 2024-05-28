#include "CrystalEditor.h"

namespace CE::Editor
{

    SceneEditorWindow::SceneEditorWindow()
    {
	    
    }

    SceneEditorWindow::~SceneEditorWindow()
    {
	    
    }

    void SceneEditorWindow::Construct()
    {
	    Super::Construct();

        SetTitle("DefaultScene");
        SetAsMainWindow(true);

        rendererSubsystem = gEngine->GetSubsystem<RendererSubsystem>();

        // - Menu -

        CMenuItem* fileMenuItem = CreateObject<CMenuItem>(this, "FileMenuItem");
        fileMenuItem->SetText("File");
	    {
            CMenu* fileMenu = CreateObject<CMenu>(fileMenuItem, "FileMenu");

            CMenuItem* newItem = CreateObject<CMenuItem>(fileMenu, "New");
            newItem->SetText("New");

            CMenuItem* openItem = CreateObject<CMenuItem>(fileMenu, "Open");
            openItem->SetText("Open");

            CMenuItem* recentItem = CreateObject<CMenuItem>(fileMenu, "Recent");
            recentItem->SetText("Recent Projects");
            {
                CMenu* recentMenu = CreateObject<CMenu>(recentItem, "RecentMenu");

                CMenuItem* project1 = CreateObject<CMenuItem>(recentMenu, "Project1");
                project1->SetText("Project 1");

                CMenuItem* project2 = CreateObject<CMenuItem>(recentMenu, "Project2");
                project2->SetText("Project 2");

                CMenuItem* project3 = CreateObject<CMenuItem>(recentMenu, "Project3");
                project3->SetText("Project 3");
            }

            CMenuItem* exit = CreateObject<CMenuItem>(fileMenu, "Exit");
            exit->SetText("Exit");

            Bind(exit, MEMBER_FUNCTION(CMenuItem, OnMenuItemClicked), [this](CMenuItem* menuItem)
                {
                    RequestEngineExit("USER_QUIT");
                });
	    }

        CMenuItem* editMenuItem = CreateObject<CMenuItem>(this, "EditMenuItem");
        editMenuItem->SetText("Edit");
        {
            CMenu* editMenu = CreateObject<CMenu>(editMenuItem, "EditMenu");

            CMenuItem* projectSettingsItem = CreateObject<CMenuItem>(editMenu, "ProjectSettingsItem");
            projectSettingsItem->SetText("Project Settings...");

            Bind(projectSettingsItem, MEMBER_FUNCTION(CMenuItem, OnMenuItemClicked), [](CMenuItem*)
                {
                    PlatformWindowInfo windowInfo{};
                    windowInfo.fullscreen = windowInfo.hidden = windowInfo.maximised = windowInfo.resizable = false;
                    windowInfo.windowFlags = PlatformWindowFlags::DestroyOnClose | PlatformWindowFlags::Utility;
                    PlatformWindow* platformWindow = PlatformApplication::Get()->CreatePlatformWindow("About Crystal Editor", 1024, 600, windowInfo);

                    platformWindow->SetBorderless(true);
                    platformWindow->SetAlwaysOnTop(true);

                    ProjectSettingsWindow* window = CreateWindow<ProjectSettingsWindow>("ProjectSettingsWindow", platformWindow);
                    window->Show();
                });

            CMenuItem* editorSettingsItem = CreateObject<CMenuItem>(editMenu, "editorSettingsItem");
            editorSettingsItem->SetText("Editor Settings...");
        }


        CMenuItem* helpMenuItem = CreateObject<CMenuItem>(this, "HelpMenuItem");
        helpMenuItem->SetText("Help");
	    {
            CMenu* helpMenu = CreateObject<CMenu>(helpMenuItem, "HelpMenu");

            CMenuItem* aboutItem = CreateObject<CMenuItem>(helpMenu, "AboutItem");
            aboutItem->SetText("About Crystal Editor");

            Bind(aboutItem, MEMBER_FUNCTION(CMenuItem, OnMenuItemClicked), [](CMenuItem*)
                {
                    PlatformWindowInfo windowInfo{};
                    windowInfo.fullscreen = windowInfo.hidden = windowInfo.maximised = windowInfo.resizable = false;
                    windowInfo.windowFlags = PlatformWindowFlags::DestroyOnClose | PlatformWindowFlags::Utility;
                    PlatformWindow* platformWindow = PlatformApplication::Get()->CreatePlatformWindow("About Crystal Editor", 640, 480, windowInfo);

                    platformWindow->SetBorderless(true);

                    AboutWindow* aboutWindow = CreateWindow<AboutWindow>("AboutWindow", platformWindow);
                    aboutWindow->Show();
                });

	    }

        // - Child Windows -

        auto minorDockSpace = CreateObject<CDockSpace>(nullptr, "MinorDockSpace");
        minorDockSpace->SetDockType(CDockType::Minor);
        AddSubWidget(minorDockSpace);

        minorDockSpace->Split(0.25f, CDockSplitDirection::Horizontal, "SplitCenter", "SplitRight");
        auto parentSplit = minorDockSpace->GetRootDockSplit();
        auto center = parentSplit->GetSplit(0);
        auto right = parentSplit->GetSplit(1);

    	minorDockSpace->Split(right, 0.6f, CDockSplitDirection::Vertical, "SplitRightTop", "SplitRightBottom");
        auto rightTop = right->GetSplit(0);
        auto rightBottom = right->GetSplit(1);

        sceneHierarchyWindow = CreateObject<SceneHierarchyWindow>(rightTop, "SceneHierarchy");

        detailsWindow = CreateObject<DetailsWindow>(rightBottom, "Details");

        Bind(sceneHierarchyWindow, MEMBER_FUNCTION(SceneHierarchyWindow, OnActorSelected),
            detailsWindow, MEMBER_FUNCTION(DetailsWindow, SetupForActor));

        minorDockSpace->Split(center, 0.4f, CDockSplitDirection::Vertical, "SplitTop", "SplitBottom");
        auto centerTop = center->GetSplit(0);
        auto centerBottom = center->GetSplit(1);

        viewportWindow = CreateObject<ViewportWindow>(centerTop, "Viewport");
        centerTop->SetAutoHideTabs(true);
        EditorViewport* editorViewport = viewportWindow->GetViewport();
        
        Bind(editorViewport, MEMBER_FUNCTION(EditorViewport, OnFrameBufferRecreated),
            rendererSubsystem, MEMBER_FUNCTION(RendererSubsystem, RebuildFrameGraph));

        assetBrowserWindow = CreateObject<AssetBrowserWindow>(centerBottom, "Assets");
    }

} // namespace CE::Editor
