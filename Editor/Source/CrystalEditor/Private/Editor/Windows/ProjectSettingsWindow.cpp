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

		SetTitle("Project Settings");
		SetFullscreen(false);

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
							editor->SetTargets({ SettingsBase::LoadSettings(settingsClass) });
						});
				}
			}
		}

		auto rightView = CreateWidget<CLayoutGroup>(splitView->GetRight(), "ProjectSettingsRightView");
		{
			auto titleLabel = CreateWidget<CLabel>(rightView, "TitleLabel");
			titleLabel->SetText("Project");
			titleLabel->AddStyleClass("Title");

			editor = CreateWidget<ObjectEditor>(rightView, "ProjectSettingsObjectEditor");
			editor->SetTargets({ GetSettings<ProjectSettings>() });
		}
	}

} // namespace CE::Editor
