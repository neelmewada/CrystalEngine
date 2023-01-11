
#include "Plugin/PluginManager.h"

namespace CE
{

	PluginManager::PluginManager()
	{

	}

	void PluginManager::Init()
	{
		ConfigManager::Get()->LoadDefaultEngineConfig<PluginConfig>(&pluginConfig); // Load engine default plugin config
		ConfigManager::Get()->LoadGameConfig("EnginePlugins", PluginConfig::Type(), &pluginConfig); // Load project specific plugin config overrides

		pluginConfig.OnAfterDeserialize();
	}

	void PluginManager::LoadPlugins(PluginLoadType loadType)
	{
		for (const auto& plugin : pluginConfig.plugins)
		{
			if (loadType == plugin.loadType && plugin.enabled)
			{
				ModuleManager::Get().LoadPluginModule(plugin.name.GetString());
			}
		}
	}

	void PluginManager::UnloadAllPlugins()
	{
		for (int i = pluginConfig.plugins.GetSize() - 1; i >= 0; i--)
		{
			ModuleManager::Get().UnloadPluginModule(pluginConfig.plugins[i].name.GetString());
		}
	}

	void PluginManager::LoadPlugin(String pluginName)
	{
		ModuleManager::Get().LoadPluginModule(pluginName);
	}

	void PluginManager::UnloadPlugin(String pluginName)
	{
		ModuleManager::Get().UnloadPluginModule(pluginName);
	}

} // namespace CE
