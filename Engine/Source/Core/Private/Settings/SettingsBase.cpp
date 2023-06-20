#include "CoreMinimal.h"

namespace CE
{

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
}

CE_RTTI_CLASS_IMPL(CORE_API, CE, SettingsBase)

