#pragma once

namespace CE
{
    class CORE_API ProjectSettings : public SettingsBase
    {
        CE_CLASS(ProjectSettings, SettingsBase)
    public:
        
        ProjectSettings() = default;
        

		String projectName = "Untitled";
		String projectVersion = "0.0.1";
		

    };
}


CE_RTTI_CLASS(CORE_API, CE, ProjectSettings,
    CE_SUPER(CE::SettingsBase),
    CE_NOT_ABSTRACT,
    CE_ATTRIBS(Settings = "ProjectSettings"),
    CE_FIELD_LIST(
		CE_FIELD(projectName)
		CE_FIELD(projectVersion)
    ),
    CE_FUNCTION_LIST()
)
