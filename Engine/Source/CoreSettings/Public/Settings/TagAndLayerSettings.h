#pragma once

namespace CE
{
    CLASS(Settings = "TagAndLayerSettings", Display = "Tags and Layers", SettingsCategory = "Project")
	class CORESETTINGS_API TagAndLayerSettings : public Settings
	{
		CE_CLASS(TagAndLayerSettings, Settings)
	public:

		virtual ~TagAndLayerSettings();

		FIELD(Category = "Tags")
		bool useTags = true;

		FIELD(Category = "Tags", Validator = "Name")
		Array<String> tags{};
	};

} // namespace CE

#include "TagAndLayerSettings.rtti.h"
