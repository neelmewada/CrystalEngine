#pragma once

#include "CoreMinimal.h"

#include "Module/ModuleManager.h"

#include "ConfigBase.h"

namespace CE
{
    struct PluginInfo
    {
        CE::Name name;
        bool enabled = false;
        PluginLoadType loadType;
    };

    struct SYSTEM_API PluginConfig : public ConfigBase
    {
        CE_STRUCT(PluginConfig, ConfigBase)

    public:

        static String GetDefaultConfigFile() { return "DefaultPlugins"; }

        String PluginList;
        String EnabledPlugins;
        String SupportedPlugins;

        String LoadEarliest;
        String LoadOnPreInit;
        String LoadOnInit;
        String LoadOnPostInit;

    public: // Non serialized

        void OnAfterDeserialize();

        Array<PluginInfo> plugins{};

    };
    
} // namespace CE

CE_RTTI_STRUCT(SYSTEM_API, CE, PluginConfig,
    CE_SUPER(CE::ConfigBase),
    CE_ATTRIBS(),
    CE_FIELD_LIST(
        CE_FIELD(PluginList)
        CE_FIELD(EnabledPlugins)
        CE_FIELD(SupportedPlugins)

        CE_FIELD(LoadEarliest)
        CE_FIELD(LoadOnPreInit)
        CE_FIELD(LoadOnInit)
        CE_FIELD(LoadOnPostInit)
    )
)
