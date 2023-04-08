#pragma once

#include "CoreMinimal.h"

namespace CE
{
    class ConfigBase;

    class SYSTEM_API ConfigManager
    {
    private:
        ConfigManager() = default;
        ~ConfigManager() = default;

    public:
        
        static ConfigManager* Get()
        {
            static ConfigManager instance{};
            return &instance;
        }

        bool LoadEditorConfig(String configFileName, StructType* structType, ConfigBase* instance);
        bool LoadEngineConfig(String configFileName, StructType* structType, ConfigBase* instance);
        bool LoadGameConfig(String configFileName, StructType* structType, ConfigBase* instance);

        template<typename ConfigType>
        bool LoadDefaultEngineConfig(ConfigType* instance)
        {
            return LoadEngineConfig(ConfigType::GetDefaultConfigFile(), ConfigType::Type(), instance);
        }
        
    private:

    };

} // namespace CE
