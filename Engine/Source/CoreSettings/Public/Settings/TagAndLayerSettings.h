#pragma once

namespace CE
{
    CLASS(Settings = "TagAndLayerSettings", Display = "Tags and Layers", SettingsCategory = "Project")
	class CORESETTINGS_API TagAndLayerSettings : public SettingsBase
	{
		CE_CLASS(TagAndLayerSettings, SettingsBase)
	public:

		virtual ~TagAndLayerSettings();

		FIELD(Category = "Tags")
		bool useTags = true;

		FIELD(Category = "Tags")
		Array<String> tags{};
	};

} // namespace CE

#include "TagAndLayerSettings.rtti.h"
