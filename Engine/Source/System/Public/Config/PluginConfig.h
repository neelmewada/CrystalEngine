#pragma once

#include "Containers/Array.h"
#include "Object/Object.h"

namespace CE
{
    enum class PluginLoadType
    {
        DontLoad,
        LoadOnPreInit,
        LoadOnInit,
        LoadOnPostInit
    };

    struct PluginInfo
    {
        String name;
        bool enabledByDefault = false;
        PluginLoadType loadType;
    };

    struct SYSTEM_API PluginConfig
    {
        CE_STRUCT(PluginConfig)

    public:

        String PluginList;
        String EnabledPlugins;

        String LoadOnPreInit;
        String LoadOnInit;
        String LoadOnPostInit;

    public: // Non serialized

        void Validate();

        Array<PluginInfo> plugins{};

    };
    
} // namespace CE

CE_RTTI_STRUCT(SYSTEM_API, CE, PluginConfig,
    CE_SUPER(),
    CE_ATTRIBS(),
    CE_FIELD_LIST(
        CE_FIELD(PluginList)
        CE_FIELD(EnabledPlugins)

        CE_FIELD(LoadOnPreInit)
        CE_FIELD(LoadOnInit)
        CE_FIELD(LoadOnPostInit)
    )
)
