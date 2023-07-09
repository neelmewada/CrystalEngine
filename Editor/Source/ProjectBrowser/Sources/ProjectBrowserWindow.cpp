
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
	height: 200px;
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

#ProjectFolderLabel, #ProjectNameLabel {
	width: 120px;
	flex-wrap: wrap;
	text-align: middle-left;
	margin: 10px 0 0 0;
}

#FolderPathInput, #ProjectNameInput {
	width: 300px;
}


)";

void ProjectBrowserWindow::Construct()
{
	isFullscreen = true;

	Super::Construct();
	
	SetStyleSheet(css);
	
	CTabWidget* tabWidget = CreateWidget<CTabWidget>(this, "ProjectBrowserTabWidget");

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
			auto label = CreateWidget<CLabel>(container, "ProjectFolderLabel");
			label->SetText("Project Folder");
			folderPathInput = CreateWidget<CTextInput>(container, "FolderPathInput");
			folderPathInput->AddStyleClass("Horizontal");
		}

		// Project Name
		{
			//auto label = CreateWidget<CLabel>(container, "ProjectNameLabel");
			//label->SetText("Project Name");
			//projectNameInput = CreateWidget<CTextInput>(container, "ProjectNameInput");
			//projectNameInput->AddStyleClass("Horizontal");
		}
	}

	// Open Project Tab
	{
		auto container = tabWidget->GetOrAddTab("Open Project");

	}

}

