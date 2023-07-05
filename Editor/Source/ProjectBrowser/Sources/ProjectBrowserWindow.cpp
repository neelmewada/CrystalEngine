
#include "Main.h"

void ProjectBrowserWindow::Construct()
{
	isFullscreen = true;

	Super::Construct();
	
	/*auto tabView = CreateWidget<CTabView>(this, "TabView");

	createContent = CreateWidget<CStackLayout>(tabView, "CreateProjectContent");
	createContent->SetDirection(CStackDirection::Vertical);
	createContent->GetStyle().AddProperty(CStylePropertyType::Height, CStyleValue(100, true));
	createContent->GetStyle().AddProperty(CStylePropertyType::Padding, Vec4(10, 10, 10, 10));

	// Create Project Tab
	{
		auto header = CreateWidget<CLabel>(createContent, "HeaderLabel");
		header->SetText("Select Template");
		header->GetStyle().AddProperty(CStylePropertyType::FontSize, 20);
		header->GetStyle().AddProperty(CStylePropertyType::Height, 30);

		auto templateContent = CreateWidget<CSelectableGroup>(createContent, "Templates");
		templateContent->SetDirection(CStackDirection::Vertical);
		templateContent->GetStyle().AddProperty(CStylePropertyType::Padding, Vec4(10, 10, 10, 10));
		templateContent->GetStyle().AddProperty(CStylePropertyType::MaxHeight, 250);
		templateContent->GetStyle().AddProperty(CStylePropertyType::FontSize, 18);
		templateContent->GetStyle().AddProperty(CStylePropertyType::Background, Color(0.0f, 0.0f, 0.0f, 0.25f));

		for (int i = 0; i < 1; i++)
		{
			auto selectable = CreateWidget<CSelectableWidget>(templateContent, "TemplateItem");
			selectable->GetStyle().AddProperty(CStylePropertyType::MinHeight, 40);
			if (i == 0)
				selectable->Select(true);

			auto subLabel = CreateWidget<CLabel>(selectable, "SubLabel");
			subLabel->SetText(String::Format("Empty Project", i));
			subLabel->GetStyle().AddProperty(CStylePropertyType::Height, 40);
		}

		auto projectPathStack = CreateWidget<CStackLayout>(createContent, "ProjectPathStackLayout");
		projectPathStack->SetDirection(CStackDirection::Horizontal);

		auto projectPathLabel = CreateWidget<CLabel>(projectPathStack, "ProjectPathLabel");
		projectPathLabel->SetText("Project Folder");
		projectPathLabel->GetStyle().AddProperty(CStylePropertyType::Height, 30);
		projectPathLabel->GetStyle().AddProperty(CStylePropertyType::MinWidth, 100);
		projectPathLabel->GetStyle().AddProperty(CStylePropertyType::TextAlign, Alignment::MiddleLeft);

		folderPathInput = CreateWidget<CTextInput>(projectPathStack, "ProjectPathInput");
		folderPathInput->SetHint("Project folder path...");
		folderPathInput->GetStyle().AddProperty(CStylePropertyType::Width, CStyleValue(90, true));

		Object::Bind(folderPathInput, MEMBER_FUNCTION(CTextInput, OnValueChanged), [](String string)
			{
				CE_LOG(Info, All, "Text: " + string);
			});

		auto folderPathButton = CreateWidget<CButton>(projectPathStack, "ProjectPathButton");
		folderPathButton->SetText("Open");
		folderPathButton->GetStyle().AddProperty(CStylePropertyType::MinHeight, 15);
		folderPathButton->GetStyle().AddProperty(CStylePropertyType::Padding, Vec4(5, 2.5f, 5, 2.5f));
	}

	openContent = CreateWidget<CStackLayout>(tabView, "OpenProjectContent");
	openContent->SetDirection(CStackDirection::Vertical);
	openContent->GetStyle().AddProperty(CStylePropertyType::Padding, Vec4(10, 5, 10, 10));

	auto label2 = CreateWidget<CLabel>(openContent, "Label2");
	label2->SetText("This is the second tab.");

	tabView->SetTabTitle(createContent, "Create Project");
	tabView->SetTabTitle(openContent, "Open Project");

	tabView->GetStyle().AddProperty(CStylePropertyType::Padding, Vec4(20, 10, 20, 10), CStateFlag::Default, CSubControl::Tab);
	*/
}

