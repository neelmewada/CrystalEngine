#pragma once

namespace CE
{
    CLASS(Settings = "ProjectSettings", Display = "Project", SettingsCategory = "Project")
    class CORESETTINGS_API ProjectSettings : public Settings
    {
        CE_CLASS(ProjectSettings, Settings)
    public:
        
        ProjectSettings() = default;

        // Description
        
        FIELD(Category = "Description", Validator = "Name")
		String projectName = "Untitled";

        FIELD(Category = "Description", Validator = "LongVersion")
		String projectVersion = "0.0.1";

        FIELD(Category = "Description")
		String description = "";

		// Legal

        FIELD(Category = "Legal")
		String copyrightNotice = "";

        FIELD(Category = "Legal", Display = "Privacy Policy URL")
		String privacyPolicyUrl = "";
    };
}

#include "ProjectSettings.rtti.h"