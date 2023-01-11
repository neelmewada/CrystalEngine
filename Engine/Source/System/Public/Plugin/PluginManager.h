#pragma once

#include "Config/ConfigManager.h"
#include "Config/PluginConfig.h"

namespace CE
{
    
    class SYSTEM_API PluginManager
    {
    private:
        PluginManager();
        ~PluginManager() = default;

    public:

        static PluginManager& Get()
        {
            static PluginManager instance{};
            return instance;
        }

        void Init();

        void LoadPlugins(PluginLoadType loadType);

        void UnloadAllPlugins();

        void LoadPlugin(String pluginName);
        void UnloadPlugin(String pluginName);

    private:
        PluginConfig pluginConfig{};
    };

} // namespace CE
