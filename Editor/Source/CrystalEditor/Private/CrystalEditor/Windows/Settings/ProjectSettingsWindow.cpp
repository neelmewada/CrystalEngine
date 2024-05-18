#include "CrystalEditor.h"

namespace CE::Editor
{

	ProjectSettingsWindow::ProjectSettingsWindow()
	{
		title = "Project Settings";

		canBeClosed = true;
		canBeMaximized = canBeMinimized = true;
	}

	ProjectSettingsWindow::~ProjectSettingsWindow()
	{
		
	}

	void ProjectSettingsWindow::OnPlatformWindowSet()
	{
		Super::OnPlatformWindowSet();

		if (nativeWindow)
		{
			nativeWindow->SetAlwaysOnTop(true);
			nativeWindow->platformWindow->SetResizable(true);
		}
	}

	void ProjectSettingsWindow::Construct()
	{
		Super::Construct();

		LoadStyleSheet(PlatformDirectories::GetLaunchDir() / "Editor/Styles/ProjectSettingsStyle.css");

		sideBar = CreateObject<CWidget>(this, "SideBar");
		sideBar->SetVerticalScrollAllowed(true);
		sideBar->AddBehavior<CScrollBehavior>();

		content = CreateObject<CWidget>(this, "ContentView");
		content->SetVerticalScrollAllowed(true);
		content->AddBehavior<CScrollBehavior>();

		const Array<ClassType*>& settingsClasses = SettingsBase::GetAllSettingsClasses();

		struct SettingsEntry
		{
			Name category{};
			Name name{};
			ClassType* settingsClass = nullptr;
		};

		Array<Name> settingCategories{};
		HashSet<Name> visitedCategories{};

		HashMap<Name, Array<SettingsEntry>> settingsByCategory{};

		for (ClassType* settingsClass : settingsClasses)
		{
			if (!settingsClass->HasAttribute("SettingsCategory"))
				continue;

			Name category = settingsClass->GetAttribute("SettingsCategory").GetStringValue();
			if (!category.IsValid())
				continue;

			Name name{};

			if (settingsClass->HasAttribute("Settings"))
			{
				name = settingsClass->GetAttribute("Settings").GetStringValue();
			}

			if (!name.IsValid())
			{
				name = settingsClass->GetName().GetLastComponent();
			}

			if (!visitedCategories.Exists(category))
			{
				settingCategories.Add(category);
				visitedCategories.Add(category);
			}

			settingsByCategory[category].Add({ .category = category, .name = name, .settingsClass = settingsClass });
		}

		for (Name settingCategory : settingCategories)
		{
			CLabel* title = CreateObject<CLabel>(sideBar, "SideBarTitle");
			title->SetText(settingCategory.GetString());

			for (const auto& settingsEntry : settingsByCategory[settingCategory])
			{
				CLabel* entry = CreateObject<CLabel>(sideBar, "SideBarEntryLabel");
				entry->SetText(settingsEntry.settingsClass->GetDisplayName());
			}
		}
	}

} // namespace CE::Editor
