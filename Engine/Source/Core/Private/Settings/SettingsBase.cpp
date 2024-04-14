#include "CoreMinimal.h"

namespace CE
{
	Array<ClassType*> SettingsBase::settingsClasses{};

	static SettingsBase* LoadSettingsFromPackage(ClassType* settingsClass)
	{
		if (!settingsClass->IsSubclassOf<SettingsBase>() || !settingsClass->CanBeInstantiated())
			return nullptr;
		return (SettingsBase*)GetSettingsPackage()->LoadObject(settingsClass->GetTypeName());
	}

    SettingsBase* SettingsBase::LoadSettings(ClassType* settingsClass, String settingsName)
    {
        if (!settingsClass->IsSubclassOf<SettingsBase>() || !settingsClass->CanBeInstantiated())
            return nullptr;
		
		SettingsBase* settings = LoadSettingsFromPackage(settingsClass);

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

			settings = CreateObject<SettingsBase>(GetSettingsPackage(), settingsName, OF_NoFlags, settingsClass);
		}
		
		return settings;
    }

	void SettingsBase::SaveSettings()
	{
		Package* settingsPackage = GetSettingsPackage();
		if (settingsPackage == nullptr)
			return;

		if (!settingsPackage->IsFullyLoaded())
			settingsPackage->LoadFully();

		Package::SavePackage(settingsPackage, nullptr);
	}

	void SettingsBase::SaveSettings(const IO::Path& customPath)
	{
		Package* settingsPackage = GetSettingsPackage();
		if (settingsPackage == nullptr)
			return;

		if (!settingsPackage->IsFullyLoaded())
			settingsPackage->LoadFully();

		Package::SavePackage(settingsPackage, nullptr, customPath);
	}

	Array<ClassType*> SettingsBase::GetSettingsClassesWithCategory(const String& settingsCategory)
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

	void SettingsBase::OnClassRegistered(ClassType* classType)
	{
		if (classType != nullptr && classType->IsSubclassOf<SettingsBase>() && classType != GetStaticClass<SettingsBase>())
		{
			settingsClasses.Add(classType);
		}
	}

	void SettingsBase::OnClassDeregistered(ClassType* classType)
	{
		if (classType != nullptr && classType->IsSubclassOf<SettingsBase>() && classType != GetStaticClass<SettingsBase>())
		{
			settingsClasses.Remove(classType);
		}
	}
}

CE_RTTI_CLASS_IMPL(CORE_API, CE, SettingsBase)

