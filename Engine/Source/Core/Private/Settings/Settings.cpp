#include "CoreMinimal.h"

namespace CE
{
	Array<ClassType*> Settings::settingsClasses{};

	static Settings* LoadSettingsFromBundle(ClassType* settingsClass)
	{
		if (!settingsClass->IsSubclassOf<Settings>() || !settingsClass->CanBeInstantiated())
			return nullptr;
		return (Settings*)GetSettingsBundle()->LoadObject(settingsClass->GetTypeName());
	}

    Settings* Settings::LoadSettings(ClassType* settingsClass, String settingsName)
    {
        if (!settingsClass->IsSubclassOf<Settings>() || !settingsClass->CanBeInstantiated())
            return nullptr;
		
		Settings* settings = LoadSettingsFromBundle(settingsClass);

		if (settings == nullptr)
		{
			if (settingsName.IsEmpty() && settingsClass->HasAttribute("Settings"))
			{
				settingsName = settingsClass->GetAttribute("Settings").GetStringValue();
			}
			if (settingsName.IsEmpty())
			{
				settingsName = settingsClass->GetName().GetLastComponent();
			}

			settings = CreateObject<Settings>(GetSettingsBundle(), settingsName, OF_NoFlags, settingsClass);
		}
		
		return settings;
    }

	void Settings::SaveSettings()
	{
		Bundle* settingsBundle = GetSettingsBundle();
		if (settingsBundle == nullptr)
			return;

		if (!settingsBundle->IsFullyLoaded())
			settingsBundle->LoadFully();

		Bundle::SaveBundleToDisk(settingsBundle, nullptr);
	}

	String Settings::GetTitleName()
	{
		return GetClass()->GetDisplayName();
	}

#if PAL_TRAIT_BUILD_EDITOR
	void Settings::SaveSettings(const IO::Path& customPath)
	{
		Bundle* settingsBundle = GetSettingsBundle();
		if (settingsBundle == nullptr)
			return;

		if (!settingsBundle->IsFullyLoaded())
			settingsBundle->LoadFully();

		Bundle::SaveBundleToDisk(settingsBundle, nullptr, customPath);
	}
#endif


	Array<ClassType*> Settings::GetSettingsClassesWithCategory(const String& settingsCategory)
    {
		Array<ClassType*> result{};
		
		for (auto classType : settingsClasses)
		{
			if (classType == nullptr)
				continue;
			if (classType->HasAttribute("SettingsCategory") && classType->GetAttribute("SettingsCategory").IsString())
			{
				String category = classType->GetAttribute("SettingsCategory").GetStringValue();
				if (category == settingsCategory)
					result.Add(classType);
			}
		}
        return result;
    }

	void Settings::OnClassRegistered(ClassType* classType)
	{
		if (classType != nullptr && classType->IsSubclassOf<Settings>() && classType != GetStaticClass<Settings>())
		{
			settingsClasses.Add(classType);
		}
	}

	void Settings::OnClassDeregistered(ClassType* classType)
	{
		if (classType != nullptr && classType->IsSubclassOf<Settings>() && classType != GetStaticClass<Settings>())
		{
			settingsClasses.Remove(classType);
		}
	}
}

CE_RTTI_CLASS_IMPL(CORE_API, CE, Settings)

