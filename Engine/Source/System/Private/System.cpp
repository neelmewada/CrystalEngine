
#include "System.h"

CE_IMPLEMENT_MODULE(System, CE::SystemModule)

namespace CE
{
    
    
    void SystemModule::StartupModule()
    {
        
    }

    void SystemModule::ShutdownModule()
    {
        
    }

    void SystemModule::RegisterTypes()
    {
        // Miscellaneous Types
        CE_REGISTER_TYPES(
            ProjectSettings
        );

        // Game Framework Types
        CE_REGISTER_TYPES(
            Engine,
            GameObject,
            GameComponent,
            Scene
        );
        
        // Configs
        CE_REGISTER_TYPES(
            ConfigBase,
            PluginConfig
        );

        // Register Components
        CE_REGISTER_TYPES(
            TransformComponent
        );
    }

} // namespace CE

