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

            CMenuItem* exit = CreateObject<CMenuItem>(fileMenu, "Exit");
            exit->SetText("Exit");
            Bind(exit, MEMBER_FUNCTION(CMenuItem, OnMenuItemClicked), [this]
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

            CMenuItem* editorSettingsItem = CreateObject<CMenuItem>(editMenu, "editorSettingsItem");
            editorSettingsItem->SetText("Editor Settings...");
        }

        // - Child Windows -

        auto minorDockSpace = CreateObject<CDockSpace>(nullptr, "MinorDockSpace");
        minorDockSpace->SetDockType(CDockType::Minor);
        AddSubWidget(minorDockSpace);

        minorDockSpace->Split(0.25f, CDockSplitDirection::Horizontal, "SplitCenter", "SplitRight");
        auto parentSplit = minorDockSpace->GetRootDockSplit();
        auto center = parentSplit->GetSplit(0);
        auto right = parentSplit->GetSplit(1);

        detailsWindow = CreateObject<DetailsWindow>(right, "Details");

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
