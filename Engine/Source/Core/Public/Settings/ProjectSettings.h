#pragma once

namespace CE
{
    class CORE_API ProjectSettings : public SettingsBase
    {
        CE_CLASS(ProjectSettings, SettingsBase)
    public:
        
        ProjectSettings() = default;
        
    };
}


CE_RTTI_CLASS(CORE_API, CE, ProjectSettings,
    CE_SUPER(CE::SettingsBase),
    CE_NOT_ABSTRACT,
    CE_ATTRIBS(Settings = "ProjectSettings"),
    CE_FIELD_LIST(
                  
    ),
    CE_FUNCTION_LIST()
)
