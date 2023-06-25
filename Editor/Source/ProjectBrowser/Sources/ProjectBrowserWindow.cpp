
#include "Main.h"

void ProjectBrowserWindow::Construct()
{
	isFullscreen = true;

	Super::Construct();
	
	auto tabView = CreateWidget<CTabView>(this, "TabView");

	createContent = CreateWidget<CStackLayout>(tabView, "CreateProjectContent");
	createContent->SetDirection(CStackDirection::Vertical);
	createContent->GetStyle().AddProperty(CStylePropertyType::Height, CStyleValue(100, true));
	createContent->GetStyle().AddProperty(CStylePropertyType::Padding, Vec4(10, 50, 10, 10));

	auto templateContent = CreateWidget<CStackLayout>(createContent, "Templates");
	templateContent->SetDirection(CStackDirection::Vertical);
	templateContent->GetStyle().AddProperty(CStylePropertyType::Padding, Vec4(10, 10, 10, 10));
	templateContent->GetStyle().AddProperty(CStylePropertyType::MaxHeight, 250);
	templateContent->GetStyle().AddProperty(CStylePropertyType::Background, Color(0.0f, 0.0f, 0.0f, 0.25f));

	//auto createGridLayout = CreateWidget<CGridLayout>(templateContent, "CreateTemplates");
	//createGridLayout->GetStyle().AddProperty(CStylePropertyType::Background, Color::FromRGBA32(0, 0, 0, 255));

	// selectable: rgb(0, 145, 197, 180)
	auto selectable0 = CreateWidget<CSelectableWidget>(templateContent, "TemplateItem");
	selectable0->GetStyle().AddProperty(CStylePropertyType::MinHeight, 40);

	auto subLabel = CreateWidget<CLabel>(selectable0, "SubLabel");
	subLabel->SetText("Empty Project");
	subLabel->GetStyle().AddProperty(CStylePropertyType::Height, 40);

	openContent = CreateWidget<CStackLayout>(tabView, "OpenProjectContent");
	openContent->SetDirection(CStackDirection::Vertical);
	openContent->GetStyle().AddProperty(CStylePropertyType::Padding, Vec4(10, 5, 10, 10));

	auto label2 = CreateWidget<CLabel>(openContent, "Label2");
	label2->SetText("This is the second tab.");

	tabView->SetTabTitle(createContent, "Create Project");
	tabView->SetTabTitle(openContent, "Open Project");

	tabView->GetStyle().AddProperty(CStylePropertyType::Padding, Vec4(20, 10, 20, 10), CStateFlag::Default, CSubControl::Tab);
}

