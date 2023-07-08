
#include "Main.h"


static const String css = R"(
ProjectBrowserWindow {
	padding: 0px 0px;
	align-items: flex-start;
}

#SelectableGroup {
	width: 100%;
	height: 200px;
	background: rgb(26, 26, 26);
	padding: 0px 0px;
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
)";

void ProjectBrowserWindow::Construct()
{
	isFullscreen = true;

	Super::Construct();
	
	SetStyleSheet(css);
	
	CTabWidget* tabWidget = CreateWidget<CTabWidget>(this, "TabWidget");

	// Create Project Tab
	{
		auto container = tabWidget->GetOrAddTab("Create Project" );

		auto label = CreateWidget<CLabel>(container, "Label");
		label->SetText("Select Template");

		auto selectableGroup = CreateWidget<CSelectableGroup>(container, "SelectableGroup");

		for (int j = 0; j < 1; j++)
		{
			auto selectableWidget = CreateWidget<CSelectableWidget>(selectableGroup, String::Format("Selectable{}", j));
			auto label = CreateWidget<CLabel>(selectableWidget, "Label");
			label->SetText("Empty Project");
		}

		selectableGroup->SelectItemAt(0);
	}

	// Open Project Tab
	{
		auto container = tabWidget->GetOrAddTab("Open Project");

	}

}

