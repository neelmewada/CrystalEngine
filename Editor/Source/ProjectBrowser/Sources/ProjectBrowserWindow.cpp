
#include "Main.h"

void ProjectBrowserWindow::Construct()
{
	isFullscreen = true;

	Super::Construct();
	
	auto tabView = CreateWidget<CTabView>(this, "TabView");

	createContent = CreateWidget<CStackLayout>(tabView, "CreateProjectContent");
	createContent->SetDirection(CStackDirection::Vertical);
	createContent->GetStyle().AddProperty(CStylePropertyType::Padding, Vec4(10, 10, 10, 10));

    auto btn1 = CreateWidget<CButton>(createContent, "Button1");
    btn1->SetText("Button 1");
	auto btn2 = CreateWidget<CButton>(createContent, "Button2");
	btn2->SetText("Button 2");

	auto selectable = CreateWidget<CSelectableWidget>(createContent, "SelectableWidget0");
	auto subLabel = CreateWidget<CLabel>(selectable, "SubLabel");
	subLabel->SetText("3D Template");

	openContent = CreateWidget<CStackLayout>(tabView, "OpenProjectContent");
	openContent->SetDirection(CStackDirection::Vertical);
	openContent->GetStyle().AddProperty(CStylePropertyType::Padding, Vec4(10, 10, 10, 10));

	auto label2 = CreateWidget<CLabel>(openContent, "Label2");
	label2->SetText("This is the second tab.");

	tabView->SetTabTitle(createContent, "Create Project");
	tabView->SetTabTitle(openContent, "Open Project");

	tabView->GetStyle().AddProperty(CStylePropertyType::Padding, Vec4(20, 10, 20, 10), CStateFlag::Default, CSubControl::Tab);
}
