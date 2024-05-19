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
        
        FIELD(EditAnywhere, Category = "Description", Validator = "Name")
		String projectName = "Untitled";

        FIELD(EditAnywhere, Category = "Description", Validator = "LongVersion")
		String projectVersion = "0.0.1";

        FIELD(EditAnywhere, Category = "Description")
		String description = "";

		// Legal

        FIELD(EditAnywhere, Category = "Legal")
		String copyrightNotice = "";

        FIELD(EditAnywhere, Category = "Legal", Display = "Privacy Policy URL")
		String privacyPolicyUrl = "";
    };
}

#include "ProjectSettings.rtti.h"