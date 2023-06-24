
#include "Main.h"

void ProjectBrowserWindow::Construct()
{
	isFullscreen = true;

	Super::Construct();

	auto& selectorStyle = gStyleManager->GetStyleGroup("CSelectableWidget#TemplateItem");
	selectorStyle.AddProperty(CStylePropertyType::Background, Color::FromRGBA32(0, 145, 197, 80), CStateFlag::Hovered);
	selectorStyle.AddProperty(CStylePropertyType::Background, Color::FromRGBA32(0, 145, 197, 180), CStateFlag::Pressed);
	selectorStyle.AddProperty(CStylePropertyType::Background, Color::FromRGBA32(0, 145, 197, 50), CStateFlag::Active);
	
	auto tabView = CreateWidget<CTabView>(this, "TabView");

	createContent = CreateWidget<CStackLayout>(tabView, "CreateProjectContent");
	createContent->SetDirection(CStackDirection::Vertical);
	createContent->GetStyle().AddProperty(CStylePropertyType::Padding, Vec4(10, 5, 10, 10));
	createContent->GetStyle().AddProperty(CStylePropertyType::MaxHeight, 200);
	createContent->GetStyle().AddProperty(CStylePropertyType::Background, Color(0.0f, 0.0f, 0.0f, 0.25f));

	auto createGridLayout = CreateWidget<CGridLayout>(createContent, "CreateTemplates");
	createGridLayout->GetStyle().AddProperty(CStylePropertyType::Background, Color::FromRGBA32(0, 0, 0, 255));

	// selectable: rgb(0, 145, 197, 180)
	auto selectable0 = CreateWidget<CSelectableWidget>(createGridLayout, "TemplateItem");

	auto subLabel = CreateWidget<CLabel>(selectable0, "SubLabel");
	subLabel->SetText("3D Template");

	openContent = CreateWidget<CStackLayout>(tabView, "OpenProjectContent");
	openContent->SetDirection(CStackDirection::Vertical);
	openContent->GetStyle().AddProperty(CStylePropertyType::Padding, Vec4(10, 5, 10, 10));

	auto label2 = CreateWidget<CLabel>(openContent, "Label2");
	label2->SetText("This is the second tab.");

	tabView->SetTabTitle(createContent, "Create Project");
	tabView->SetTabTitle(openContent, "Open Project");

	tabView->GetStyle().AddProperty(CStylePropertyType::Padding, Vec4(20, 10, 20, 10), CStateFlag::Default, CSubControl::Tab);
	tabView->GetStyle().AddProperty(CStylePropertyType::Padding, Vec4(0, 10), CStateFlag::Default);
}
