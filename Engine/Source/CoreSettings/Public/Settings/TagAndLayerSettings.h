#pragma once

namespace CE
{
    CLASS(Settings = "TagAndLayerSettings", DisplayName = "Tags and Layers", SettingsCategory = "Project")
	class CORESETTINGS_API TagAndLayerSettings : public Settings
	{
		CE_CLASS(TagAndLayerSettings, Settings)
	public:

		virtual ~TagAndLayerSettings();

		FIELD(EditAnywhere, Category = "Tags")
		bool useTags = true;

		FIELD(EditAnywhere, Category = "Tags", Validator = "Name")
		Array<String> tags{};
	};

} // namespace CE

#include "TagAndLayerSettings.rtti.h"
