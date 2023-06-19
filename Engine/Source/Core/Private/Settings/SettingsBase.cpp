#include "CoreMinimal.h"

namespace CE
{
    SettingsBase* SettingsBase::LoadSettings(ClassType* settingsClass, String settingsName)
    {
        if (!settingsClass->IsSubclassOf<SettingsBase>() || !settingsClass->CanBeInstantiated())
            return nullptr;
        
        if (settingsName.IsEmpty() && settingsClass->HasAttribute("Settings"))
        {
            settingsName = settingsClass->GetAttribute("Settings").GetStringValue();
        }
        if (settingsName.IsEmpty())
        {
            settingsName = settingsClass->GetName().GetLastComponent();
        }
        
        SettingsBase* settings = CreateObject<SettingsBase>(nullptr, settingsName);
        
        return settings;
    }
}

CE_RTTI_CLASS_IMPL(CORE_API, CE, SettingsBase)

