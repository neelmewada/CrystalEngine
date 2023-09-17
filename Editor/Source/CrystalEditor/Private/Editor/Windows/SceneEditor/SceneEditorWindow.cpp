#include "CrystalEditor.h"

namespace CE::Editor
{
    SceneEditorWindow::SceneEditorWindow()
    {
		defaultDockPosition = CDockPosition::Fill;
    }

    SceneEditorWindow::~SceneEditorWindow()
    {

    }

	void SceneEditorWindow::OpenScene(Scene* scene)
	{
		if (currentScene != nullptr)
			currentScene->Destroy();
		currentScene = scene;

		
	}

	void SceneEditorWindow::CloseScene()
	{
		if (currentScene != nullptr)
			currentScene->Destroy();
		currentScene = nullptr;
	}

    void SceneEditorWindow::Construct()
    {
		Super::Construct();

		SetAsDockSpaceWindow(true);
		SetTitle("Scene Editor");
		SetFullscreen(false);

		// Menu Bar
		{
			auto fileMenuItem = CreateWidget<CMenuItem>(menuBar, "FileMenuItem");
			fileMenuItem->SetText("File");
			auto fileMenu = CreateWidget<CMenu>(fileMenuItem, "FileMenu");
			fileMenuItem->SetSubMenu(fileMenu);
			{
				for (int i = 0; i < 8; i++)
				{
					auto item = CreateWidget<CMenuItem>(fileMenu, "FileMenuSubItem");
					item->SetText(String::Format("File Item {}", i));

					if (i == 3)
					{
						auto subMenu = CreateWidget<CMenu>(item, "SubMenu");
						item->SetSubMenu(subMenu);
						for (int j = 0; j < 4; j++)
						{
							auto subItem = CreateWidget<CMenuItem>(subMenu, "SubItem");
							subItem->SetText(String::Format("Sub Item: {}", j));

							if (j == 1)
								subItem->SetAsToggle();
						}
					}
				}
			}

			auto editMenuItem = CreateWidget<CMenuItem>(menuBar, "EditMenuItem");
			editMenuItem->SetText("Edit");
			auto editMenu = CreateWidget<CMenu>(editMenuItem, "EditMenu");
			editMenuItem->SetSubMenu(editMenu);
			{
				auto projectSettingsItem = CreateWidget<CMenuItem>(editMenu, "ProjectSettingsMenuItem");
				projectSettingsItem->SetText("Project Settings...");
				Object::Bind(projectSettingsItem, MEMBER_FUNCTION(CMenuItem, OnMenuItemClicked), [](CMenuItem* item)
					{
						CrystalEditorWindow::Get()->ShowProjectSettingsWindow();
					});
			}

			auto toolsMenuItem = CreateWidget<CMenuItem>(menuBar, "ToolsMenuItem");
			toolsMenuItem->SetText("Tools");

			auto toolsMenu = CreateWidget<CMenu>(toolsMenuItem);
			toolsMenuItem->SetSubMenu(toolsMenu);
			{
				auto widgetDebuggerItem = CreateWidget<CMenuItem>(toolsMenu, "WidgetDebuggerItem");
				widgetDebuggerItem->SetText("Widget Debugger");
				
				Object::Bind(widgetDebuggerItem, MEMBER_FUNCTION(CMenuItem, OnMenuItemClicked), [](CMenuItem* item)
					{
						GetWidgetDebugger()->Show();
					});
			}
		}

		// ToolBar
		{
			auto saveButton = CreateWidget<CButton>(toolBar, "SaveButton");
			saveButton->SetAsContainer(false);
			saveButton->SetText("Save");
			saveButton->LoadIcon("/Icons/save.png");
			saveButton->SetIconSize(22);
		}

		assetBrowserPanel = CreateWidget<AssetBrowserPanel>(this, "SceneEditorWindow_AssetBrowserPanel");
		viewportPanel = CreateWidget<RenderViewportPanel>(this, "SceneEditorWindow_ViewportPanel");
		sceneOutlinerPanel = CreateWidget<SceneOutlinerPanel>(this, "SceneEditorWindow_SceneOutlinerPanel");
		detailsPanel = CreateWidget<DetailsPanel>(this, "SceneEditorWindow_DetailsPanel");

		assetBrowserPanel->SetDefaultDockPosition(CDockPosition::Bottom);
		viewportPanel->SetDefaultDockPosition(CDockPosition::Center);
		sceneOutlinerPanel->SetDefaultDockPosition(CDockPosition::Right);
		detailsPanel->SetDefaultDockPosition(CDockPosition::RightBottom);
    }

} // namespace CE::Editor
