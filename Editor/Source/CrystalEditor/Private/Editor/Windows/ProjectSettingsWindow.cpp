#include "CrystalEditor.h"

namespace CE::Editor
{
	ProjectSettingsWindow::ProjectSettingsWindow()
	{

	}

	ProjectSettingsWindow::~ProjectSettingsWindow()
	{

	}

	void ProjectSettingsWindow::Construct()
	{
		Super::Construct();
		
		SetAsDockSpaceWindow(false);
		SetTitle("Project Settings");
		SetFullscreen(false);
		target = GetSettings<ProjectSettings>();

		splitView = CreateWidget<CSplitView>(this, "ProjectSettingsSplitView");
		splitView->SetInitialSplit(0.25f);
		splitView->SetStretchToFill(true);

		struct SettingsCategory {
			String category{};
			Array<ClassType*> settingsClasses{};
		};

		Array<SettingsCategory> settingsCategories{};
		
		auto leftView = CreateWidget<CLayoutGroup>(splitView->GetLeft(), "ProjectSettingsLeftView");
		{
			const Array<ClassType*>& allSettingsClasses = SettingsBase::GetAllSettingsClasses();

			for (ClassType* settingsClass : allSettingsClasses)
			{
				if (settingsClass == nullptr)
					continue;
				if (settingsClass->HasAttribute(CLASS_ATTRIB_SETTINGS_CATEGORY))
				{
					String category = settingsClass->GetAttribute(CLASS_ATTRIB_SETTINGS_CATEGORY).GetStringValue();
					if (settingsCategories.Exists([&](auto&& c) { return c.category == category; }))
					{
						SettingsCategory& entry = settingsCategories[settingsCategories.IndexOf([&](auto&& c) { return c.category == category; })];
						if (!entry.settingsClasses.Exists(settingsClass))
						{
							entry.settingsClasses.Add(settingsClass);
						}
					}
					else
					{
						settingsCategories.Add({ category, Array<ClassType*>{ settingsClass } });
					}
				}
			}

			for (const auto& settingsCategory : settingsCategories)
			{
				auto titleLabel = CreateWidget<CLabel>(leftView, "LeftTitleLabel");
				titleLabel->SetText(settingsCategory.category);

				for (auto settingsClass : allSettingsClasses)
				{
					auto labelButton = CreateWidget<CLabel>(leftView, "SettingsLabel");
					labelButton->SetInteractable(true);
					labelButton->SetText(settingsClass->GetDisplayName());
					Object::Bind(labelButton, MEMBER_FUNCTION(CLabel, OnTextClicked), [=]
						{
							target = SettingsBase::LoadSettings(settingsClass);
							auto editorClass = editor->GetClass();
							auto requiredEditorClass = ObjectEditor::GetObjectEditorClass(settingsClass->GetTypeName());
							if (editorClass != requiredEditorClass)
							{
								ConstructEditor();
							}
							else
							{
								editor->SetTargets({ target });
							}
						});
				}
			}
		}
		
		rightView = CreateWidget<CLayoutGroup>(splitView->GetRight(), "ProjectSettingsRightView");
		{
			editorTitleLabel = CreateWidget<CLabel>(rightView, "TitleLabel");
			editorTitleLabel->SetText(target->GetClass()->GetDisplayName());
			editorTitleLabel->AddStyleClass("Title");

			ConstructEditor();
		}
	}

	void ProjectSettingsWindow::ConstructEditor()
	{
		if (editor != nullptr)
			editor->Destroy();
		if (target == nullptr)
			return;

		editor = ObjectEditor::CreateEditorFor(target->GetClass()->GetTypeName(), rightView);
		editorTitleLabel->SetText(target->GetClass()->GetDisplayName());
		editor->SetTargets({ target });
	}

} // namespace CE::Editor
