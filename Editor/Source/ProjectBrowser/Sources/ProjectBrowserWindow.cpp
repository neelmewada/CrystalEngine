
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

#RecentProjectsView {
	width: 100%;
	height: 220px;
	background: rgb(26, 26, 26);
	padding: 0px 5px;
	margin: 0 0 0 40px;
}

#RecentProjectsButton {
	width: 100%;
	height: 50px;
    padding: 10px 6px;
	background: rgba(0, 0, 0, 0);
	flex-direction: column;
	align-items: flex-start;
	justify-content: center;
}

#RecentProjectsButton:hovered {
	background: rgb(43, 50, 58);
}

#RecentProjectsButton:pressed {
	background: rgb(65, 87, 111);
}

#RecentProjectsButton > CLabel {
	text-align: middle-left;
}

#RecentProjectsButtonTitle {
	foreground: white;
	font-size: 18px;
}

#RecentProjectsButtonSubTitle {
	foreground: rgb(180, 180, 180);
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

#ErrorLabel {
	foreground: red;
	margin: 0 0 10px 0;
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
			Object::Bind(folderPathInput, MEMBER_FUNCTION(CTextInput, OnTextEdited), [&](String text)
				{
					errorLabel->SetText("");
				});

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
			Object::Bind(projectNameInput, MEMBER_FUNCTION(CTextInput, OnTextEdited), [&](String text)
				{
					errorLabel->SetText("");
				});
		}

		// Button Group
		{
			auto layoutGroup = CreateWidget<CLayoutGroup>(container, "ButtonLayoutGroup");

			errorLabel = CreateWidget<CLabel>(layoutGroup, "ErrorLabel");
			errorLabel->SetText("");

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

		{
			auto label = CreateWidget<CLabel>(container, "TitleLabel");
			label->SetText("Recent projects");
		}

		auto scrollView = CreateWidget<CScrollView>(container, "RecentProjectsView");
		scrollView->SetAlwaysShowVerticalScroll(true);

		for (int i = cache.recentProjects.GetSize() - 1; i >= 0; i--)
		{
			IO::Path projectPath = cache.recentProjects[i];
			if (!projectPath.Exists() || projectPath.GetExtension() != ProjectManager::Get().GetProjectFileExtension())
				continue;

			String projectName = projectPath.GetFilename().RemoveExtension().GetString();

			auto item = CreateWidget<CButton>(scrollView, "RecentProjectsButton");
			item->SetText("");
			Object::Bind(item, MEMBER_FUNCTION(CButton, OnButtonClicked), [=]
				{
					OpenProject(projectPath);
				});
			
			auto titleLabel = CreateWidget<CLabel>(item, "RecentProjectsButtonTitle");
			titleLabel->SetText(projectName);

			auto subtitleLabel = CreateWidget<CLabel>(item, "RecentProjectsButtonSubTitle");
			subtitleLabel->SetText(projectPath.GetString());
		}

		// Open Project Path
		{
			auto layoutGroup = CreateWidget<CLayoutGroup>(container, "ProjectFieldLayout");

			auto label = CreateWidget<CLabel>(layoutGroup, "ProjectFolderLabel");
			label->SetText("Project Path");
			openProjectPathInput = CreateWidget<CTextInput>(layoutGroup, "FolderPathInput");
			openProjectPathInput->SetText(cache.lastProjectFolder);
			Object::Bind(openProjectPathInput, MEMBER_FUNCTION(CTextInput, OnTextEdited), [&](String text)
				{
					openErrorLabel->SetText("");
				});

			auto openButton = CreateWidget<CButton>(layoutGroup, "OpenFolderButton");
			openButton->SetText("...");

			Object::Bind(openButton, MEMBER_FUNCTION(CButton, OnButtonClicked), [&]
				{
					auto projectExtension = ProjectManager::Get().GetProjectFileExtension();

					auto path = EditorPlatform::ShowFileSelectionDialog(openProjectPathInput->GetText(), {
						EditorPlatform::FileType{ "CrystalEngine project file", { projectExtension } }
					});

					if (!path.IsEmpty())
						openProjectPathInput->SetText(path.GetString());
				});
		}

		// Button Group
		{
			auto layoutGroup = CreateWidget<CLayoutGroup>(container, "ButtonLayoutGroup");

			openErrorLabel = CreateWidget<CLabel>(layoutGroup, "ErrorLabel");
			openErrorLabel->SetText("");

			auto cancelButton = CreateWidget<CButton>(layoutGroup, "CancelButton");
			cancelButton->SetText("Cancel");
			Object::Bind(cancelButton, MEMBER_FUNCTION(CButton, OnButtonClicked), []
				{
					gOpenProjectPath = {};
					RequestEngineExit("ProjectBrowserClosed");
				});

			auto openButton = CreateWidget<CButton>(layoutGroup, "OpenButton");
			openButton->SetText("Open");
			openButton->SetAsAlternateStyle(true);
			Object::Bind(openButton, MEMBER_FUNCTION(CButton, OnButtonClicked), this, MEMBER_FUNCTION(Self, OnOpenProjectClicked));
		}

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
		errorLabel->SetText("Entered folder path does not exist");
		return;
	}

	String projectName = projectNameInput->GetText();
	if (projectName.IsEmpty() || !IsValidObjectName(projectName))
	{
		errorLabel->SetText("Project name should not contain any special characters");
		return;
	}

	IO::Path projectPath = folderPath / projectName;
	if (projectPath.Exists())
	{
		errorLabel->SetText("Project already exists at given location");
		return;
	}

	errorLabel->SetText("");

	if (ProjectManager::Get().CreateEmptyProject(projectPath, projectName))
	{
		cache.lastProjectFolder = folderPath.GetString();
		cache.lastProjectName = projectName;

		gOpenProjectPath = projectPath / (projectName + ProjectManager::Get().GetProjectFileExtension());
		bool recentProjectExists = false;
		
		for (int i = 0; i < cache.recentProjects.GetSize(); i++)
		{
			IO::Path path = cache.recentProjects[i];
			if (path == gOpenProjectPath)
			{
				recentProjectExists = true;
				break;
			}
		}

		if (!recentProjectExists)
		{
			cache.recentProjects.Add(gOpenProjectPath.GetString());
		}
	}

	SaveCache();
	RequestEngineExit("ProjectBrowserClosed");
}

void ProjectBrowserWindow::OnOpenProjectClicked()
{
	IO::Path projectPath = openProjectPathInput->GetText();

	if (!projectPath.Exists() || projectPath.GetExtension() != ProjectManager::Get().GetProjectFileExtension())
	{
		openErrorLabel->SetText("Project file path is invalid");
		return;
	}

	openErrorLabel->SetText("");

	gOpenProjectPath = projectPath;

	for (int i = 0; i < cache.recentProjects.GetSize(); i++)
	{
		IO::Path path = cache.recentProjects[i];
		if (path == gOpenProjectPath)
		{
			cache.recentProjects.RemoveAt(i);
			break;
		}
	}

	cache.recentProjects.Add(gOpenProjectPath.GetString());

	SaveCache();
	RequestEngineExit("ProjectBrowserClosed");
}

void ProjectBrowserWindow::OpenProject(const IO::Path& projectPath)
{
	if (!projectPath.Exists() || projectPath.GetExtension() != ProjectManager::Get().GetProjectFileExtension())
	{
		openErrorLabel->SetText("Project file path is invalid");
		return;
	}

	openErrorLabel->SetText("");

	gOpenProjectPath = projectPath;

	for (int i = 0; i < cache.recentProjects.GetSize(); i++)
	{
		IO::Path path = cache.recentProjects[i];
		if (path == gOpenProjectPath)
		{
			cache.recentProjects.RemoveAt(i);
			break;
		}
	}

	cache.recentProjects.Add(gOpenProjectPath.GetString());

	SaveCache();
	RequestEngineExit("ProjectBrowserClosed");
}

