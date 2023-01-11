
#include "Config/PluginConfig.h"

namespace CE
{

	void PluginConfig::OnAfterDeserialize()
	{
		auto pluginList = PluginList.Split(',');
		auto enabledPlugins = EnabledPlugins.Split(',');
		
		auto loadEarliest = LoadEarliest.Split(',');
		auto loadOnPreInit = LoadOnPreInit.Split(',');
		auto loadOnInit = LoadOnInit.Split(',');
		auto loadOnPostInit = LoadOnPostInit.Split(',');

		for (int i = 0; i < pluginList.GetSize(); i++)
		{
			PluginInfo pluginInfo{};
			
			String name = pluginList[i];
			pluginInfo.name = name;
			pluginInfo.enabled = enabledPlugins.Exists(name);

			if (loadEarliest.Exists(name))
			{
				pluginInfo.loadType = PluginLoadType::LoadEarliest;
			}
			else if (loadOnPreInit.Exists(name))
			{
				pluginInfo.loadType = PluginLoadType::LoadOnPreInit;
			}
			else if (loadOnInit.Exists(name))
			{
				pluginInfo.loadType = PluginLoadType::LoadOnInit;
			}
			else if (loadOnPostInit.Exists(name))
			{
				pluginInfo.loadType = PluginLoadType::LoadOnPostInit;
			}

			this->plugins.Add(pluginInfo);
		}
	}

} // namespace CE

CE_RTTI_STRUCT_IMPL(SYSTEM_API, CE, PluginConfig)
