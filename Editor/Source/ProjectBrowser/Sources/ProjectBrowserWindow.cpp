
#include "Main.h"

void ProjectBrowserWindow::Construct()
{
	isFullscreen = true;

	Super::Construct();
	
	auto tabView = CreateWidget<CTabView>(this, "TabView");

	createContent = CreateWidget<CStackLayout>(tabView, "CreateProjectContent");
	createContent->SetDirection(CStackDirection::Vertical);

	auto label1 = CreateWidget<CLabel>(createContent, "Label1");
	label1->SetText("This is the first tab. And this is a long label");

	openContent = CreateWidget<CStackLayout>(tabView, "OpenProjectContent");
	openContent->SetDirection(CStackDirection::Vertical);

	auto label2 = CreateWidget<CLabel>(openContent, "Label2");
	label2->SetText("This is the second tab.");

	tabView->SetTabTitle(createContent, "Create Project");
	tabView->SetTabTitle(openContent, "Open Project");

	tabView->GetStyle().AddProperty(CStylePropertyType::Padding, Vec4(20, 10), CStateFlag::Default, CSubControl::Tab);
}
