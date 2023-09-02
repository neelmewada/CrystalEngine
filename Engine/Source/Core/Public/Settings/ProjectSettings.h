#pragma once

namespace CE
{
    class CORE_API ProjectSettings : public SettingsBase
    {
        CE_CLASS(ProjectSettings, SettingsBase)
    public:
        
        ProjectSettings() = default;
        
		// Description
		String projectName = "Untitled";
		String projectVersion = "0.0.1.0";
		String description = "";

		// Legal
		String copyrightNotice = "";
		String privacyPolicyUrl = "";
    };
}


CE_RTTI_CLASS(CORE_API, CE, ProjectSettings,
    CE_SUPER(CE::SettingsBase),
    CE_NOT_ABSTRACT,
    CE_ATTRIBS(Settings = "ProjectSettings", Display = "Project", SettingsCategory = "Project"),
    CE_FIELD_LIST(
		CE_FIELD(projectName, Category = "Description")
		CE_FIELD(projectVersion, Category = "Description")
		CE_FIELD(description, Category = "Description")

		CE_FIELD(copyrightNotice, Category = "Legal")
		CE_FIELD(privacyPolicyUrl, Category = "Legal")

    ),
    CE_FUNCTION_LIST()
)
