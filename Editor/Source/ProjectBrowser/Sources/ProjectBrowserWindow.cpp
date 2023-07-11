
#include "Main.h"


static const String css = R"(
ProjectBrowserWindow {
	padding: 0px 0px;
}

#TitleLabel {
	font-size: 20px;
	text-align: middle-left;
	margin: 5px 15px;
}

#SelectableGroup {
	width: 100%;
	height: 220px;
	background: rgb(26, 26, 26);
	padding: 0px 5px;
	margin: 0 0 0 40px;
}

#SelectableGroup > CSelectableWidget {
	width: 100%;
	height: 35px;
    padding: 0;
}

.Selectable {
	padding: 10px 0px;
	foreground: white;
	font-size: 18;
}

.Selectable:hovered {
	background: rgb(43, 50, 58);
}
.Selectable:pressed, .Selectable:active {
	background: rgb(65, 87, 111);
}

.Selectable > CLabel {
	background: none;
	height: 100%;
	text-align: middle-left;
}

#ProjectFieldLayout {
	width: 100%;
	align-items: center;
	flex-direction: row;
	flex-wrap: nowrap;
	margin: 0 0 0 10px;
	padding: 0 0 10px 0;
}

#ButtonLayoutGroup {
	width: 100%;
	justify-content: flex-end;
	flex-direction: row;
	flex-wrap: nowrap;
	padding: 0 0 10px 0;
	column-gap: 5px;
}

#ProjectFolderLabel, #ProjectNameLabel {
	width: 15%;
	height: 100%;
	text-align: middle-left;
	margin: 10px 0 0 0;
}

#FolderPathInput {
	width: 80%;
}

#OpenFolderButton {
	margin: 5px 0 0 0;
	width: 3%;
	height: 25px;
}

#ProjectNameInput {
	width: 83.5%;
}


)";

void ProjectBrowserWindow::Construct()
{
	isFullscreen = true;

	Super::Construct();
	
	SetStyleSheet(css);
	
	CTabWidget* tabWidget = CreateWidget<CTabWidget>(this, "ProjectBrowserTabWidget");

	EditorPrefs::Get().GetStruct("ProjectBrowser.Cache", &cache);

	// Create Project Tab
	{
		auto container = tabWidget->GetOrAddTab("Create Project");

		{
			auto label = CreateWidget<CLabel>(container, "TitleLabel");
			label->SetText("Select a template");
		}

		auto selectableGroup = CreateWidget<CSelectableGroup>(container, "SelectableGroup");
		selectableGroup->SetAlwaysShowVerticalScroll(true);

		for (int j = 0; j < 1; j++)
		{
			auto selectableWidget = CreateWidget<CSelectableWidget>(selectableGroup, String::Format("Selectable{}", j));
			auto label = CreateWidget<CLabel>(selectableWidget, "Label");
			label->SetText("Empty Project");
		}

		selectableGroup->SelectItemAt(0);

		// Project Folder
		{
			auto layoutGroup = CreateWidget<CLayoutGroup>(container, "ProjectFieldLayout");
			
			auto label = CreateWidget<CLabel>(layoutGroup, "ProjectFolderLabel");
			label->SetText("Project Folder");
			folderPathInput = CreateWidget<CTextInput>(layoutGroup, "FolderPathInput");
			folderPathInput->SetText(cache.lastProjectFolder);

			auto openButton = CreateWidget<CButton>(layoutGroup, "OpenFolderButton");
			openButton->SetText("...");
			
			Object::Bind(openButton, MEMBER_FUNCTION(CButton, OnButtonClicked), [&]
				{
					auto path = EditorPlatform::ShowSelectDirectoryDialog(folderPathInput->GetText());
					if (!path.IsEmpty())
						folderPathInput->SetText(path.GetString());
				});
		}

		// Project Name
		{
			auto layoutGroup = CreateWidget<CLayoutGroup>(container, "ProjectFieldLayout");

			auto label = CreateWidget<CLabel>(layoutGroup, "ProjectNameLabel");
			label->SetText("Project Name");
			projectNameInput = CreateWidget<CTextInput>(layoutGroup, "ProjectNameInput");
			projectNameInput->SetText(cache.lastProjectName);
		}

		// Button Group
		{
			auto layoutGroup = CreateWidget<CLayoutGroup>(container, "ButtonLayoutGroup");

			auto cancelButton = CreateWidget<CButton>(layoutGroup, "CancelButton");
			cancelButton->SetText("Cancel");
			Object::Bind(cancelButton, MEMBER_FUNCTION(CButton, OnButtonClicked), []
				{
					RequestEngineExit("ProjectBrowserClosed");
				});

			auto createButton = CreateWidget<CButton>(layoutGroup, "CreateButton");
			createButton->SetText("Create");
			createButton->SetAsAlternateStyle(true);
			Object::Bind(createButton, MEMBER_FUNCTION(CButton, OnButtonClicked), this, MEMBER_FUNCTION(Self, OnCreateProjectClicked));
		}
	}

	// Open Project Tab
	{
		auto container = tabWidget->GetOrAddTab("Open Project");

	}

}

void ProjectBrowserWindow::SaveCache()
{
	EditorPrefs::Get().SetStruct("ProjectBrowser.Cache", &cache);
}

void ProjectBrowserWindow::OnCreateProjectClicked()
{
	IO::Path folderPath = folderPathInput->GetText();
	if (!folderPath.Exists())
	{
		CE_LOG(Error, All, "Invalid project folder path: {}", folderPath);
		return;
	}

	String projectName = projectNameInput->GetText();
	if (projectName.IsEmpty() || !IsValidObjectName(projectName))
	{
		CE_LOG(Error, All, "Invalid project name: {}", projectName);
		return;
	}

	cache.lastProjectFolder = folderPath.GetString();
	cache.lastProjectName = projectName;
	SaveCache();

	RequestEngineExit("ProjectBrowserClosed");
}

