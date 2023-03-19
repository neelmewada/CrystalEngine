
#include "System.h"

CE_IMPLEMENT_MODULE(System, CE::SystemModule)

namespace CE
{
    
    
    void SystemModule::StartupModule()
    {
        AssetDatabase::Get().Initialize();
    }

    void SystemModule::ShutdownModule()
    {
        AssetDatabase::Get().Shutdown();
    }

    void SystemModule::RegisterTypes()
    {
        // Miscellaneous Types
        CE_REGISTER_TYPES(
            
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

        // Assets
        CE_REGISTER_TYPES(
            Asset
        );

        // Register Game Components
        CE_REGISTER_GAMECOMPONENTS(
            TransformComponent
        );
    }

} // namespace CE

